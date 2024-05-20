#ifndef INJECTOR_H
#define INJECTOR_H

#include <windows.h>
#include <string>

bool CreateRemoteThreadInject(DWORD processID, const char* dllPath);

#endif // INJECTOR_H
