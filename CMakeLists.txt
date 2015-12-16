cmake_minimum_required(VERSION 2.8)

project(OpenBangla-Keyboard)
SET(APP_NAME "OpenBangla-Keyboard")

## Set Compiler Standard 
set(CMAKE_CXX_STANDARD 11)

# Get system information. WIN32 and APPLE are defined by CMake
IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  SET(OS_LINUX 1)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")

# For Linux.
IF(OS_LINUX)
  ## Find iBus ##
  FIND_PACKAGE(PkgConfig)
  PKG_CHECK_MODULES(IBUS REQUIRED ibus-1.0)
ENDIF(OS_LINUX)

ADD_SUBDIRECTORY(src)
