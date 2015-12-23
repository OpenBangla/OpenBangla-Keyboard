cmake_minimum_required(VERSION 2.8)

project(OpenBangla-Keyboard)
set(APP_NAME "OpenBangla-Keyboard")

## Set Compiler Standard
set(CMAKE_CXX_STANDARD 11)

# Get system information. WIN32 and APPLE are defined by CMake
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(OS_LINUX 1)
endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

# For Linux.
if(OS_LINUX)
  # Include our Linux specific additions
  include(CMakeLinux.txt)
  
  include(FindPkgConfig)
  ## Find iBus ##
  find_package(PkgConfig)
  pkg_check_modules(IBUS REQUIRED ibus-1.0)
endif(OS_LINUX)

add_subdirectory(src)
