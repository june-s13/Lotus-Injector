cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Lotus_Injector)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "C:/Projects/Lotus-Injector/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Find GTKmm
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)

# Include directories
include_directories(${GTKMM_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${GTKMM_LIBRARY_DIRS})

# Add the executable
add_executable(Lotus_Injector src/main.cpp src/injector.cpp src/mainwindow.cpp)

# Link GTKmm libraries
target_link_libraries(Lotus_Injector ${GTKMM_LIBRARIES})
