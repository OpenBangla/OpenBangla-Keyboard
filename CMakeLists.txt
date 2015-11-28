cmake_minimum_required(VERSION 2.8)

project(OpenBangla-Keyboard)
SET(APP_NAME "OpenBangla-Keyboard")

# Get system information. WIN32 and APPLE are defined by CMake
IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  SET(OS_LINUX 1)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")

# For Linux.
IF(OS_LINUX)
  ## Find GLib and iBus ##
  FIND_PACKAGE(PkgConfig)
  PKG_CHECK_MODULES(GLIB2 REQUIRED glib-2.0)
  PKG_CHECK_MODULES(IBUS REQUIRED ibus-1.0)
ENDIF(OS_LINUX)

## Set Compiler flags
SET(CMAKE_CXX_FLAGS "-std=c++11" ${CMAKE_CXX_FLAGS})

ADD_SUBDIRECTORY(src)
