cmake_minimum_required(VERSION 2.6.2)

project(OpenBangla-Keyboard)
SET(APP_NAME "OpenBangla Keyboard")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Get system information. WIN32 and APPLE are defined by CMake
IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  SET(OS_LINUX 1)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")

# For Linux. In Linux, we use cmake-fedora's ManageDependency module
IF(OS_LINUX)
INCLUDE(ManageDependency)

SET(IBUS_MIN_VER "1.3")

########## Package Dependencies ##########

MANAGE_DEPENDENCY(BUILD_REQUIRES CMAKE VER "2.6.2" REQUIRED)

### ibus
SET(IBUS_PKG_CONFIG "ibus-1.0" CACHE STRING "ibus pkg-config name")
MANAGE_DEPENDENCY(REQUIRES IBUS VER "${IBUS_MIN_VER}" REQUIRED PKG_CONFIG ${IBUS_PKG_CONFIG})
MANAGE_DEPENDENCY(BUILD_REQUIRES IBUS VER "${IBUS_MIN_VER}" REQUIRED
    PKG_CONFIG ${IBUS_PKG_CONFIG} DEVEL
    )

### GLib
SET(GLIB2_PKG_CONFIG "gio-2.0;glib-2.0;gmodule-2.0;gobject-2.0;gthread-2.0"
    CACHE STRING "glib2 pkg-config name")
MANAGE_DEPENDENCY(BUILD_REQUIRES GLIB2 REQUIRED
    PKG_CONFIG ${GLIB2_PKG_CONFIG} glib-2.0 DEVEL
    )

ENDIF(OS_LINUX)

ADD_SUBDIRECTORY(src)
