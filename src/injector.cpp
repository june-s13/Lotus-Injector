#include "injector.h"
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

bool CreateRemoteThreadInject(DWORD processID, const char* dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (hProcess == NULL) {
        std::cerr << "OpenProcess failed: " << GetLastError() << std::endl;
        return false;
    }

    LPVOID pDllPath = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (pDllPath == NULL) {
        std::cerr << "VirtualAllocEx failed: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    if (!WriteProcessMemory(hProcess, pDllPath, (LPVOID)dllPath, strlen(dllPath) + 1, NULL)) {
        std::cerr << "WriteProcessMemory failed: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HMODULE hKernel32 = GetModuleHandle("Kernel32");
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, 
        (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA"), 
        pDllPath, 0, NULL);

    if (hThread == NULL) {
        std::cerr << "CreateRemoteThread failed: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);
    VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return true;
}
