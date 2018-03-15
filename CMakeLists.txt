cmake_minimum_required(VERSION 3.0.2)

project(OpenBangla-Keyboard VERSION 1.2.0)
set(APP_NAME "OpenBangla-Keyboard")

## Set versions
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

## Set Compiler Standard
set(CMAKE_CXX_STANDARD 11)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Network REQUIRED)

# Get system information. WIN32 and APPLE are defined by CMake
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(OS_LINUX 1)
endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

# Set build type to "Release" if user did not specify any build type yet
# Other possible values: Debug, Release, RelWithDebInfo and MinSizeRel
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDEBUG)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

# Sanity check our source directory to make sure that we are not trying to
# generate an in-tree build (it pollutes the source tree with a lot of CMake
# related files).
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed.\nPlease create a build directory and initiate cmake from there.")
endif()

# For Linux.
if(OS_LINUX)
  ## Additions for Linux ##
  ## Set Directories for installing and storing files
  set(CMAKE_INSTALL_PREFIX "/usr")

  if(NOT DEFINED datadir)
    set(datadir "${CMAKE_INSTALL_PREFIX}/share")
  endif()
  add_definitions(-DDATADIR="${datadir}")

  if(NOT DEFINED LIBEXEC_DIR)
    set(LIBEXEC_DIR "${CMAKE_INSTALL_PREFIX}/libexec")
  endif()
  add_definitions(-DLIBEXEC_DIR="${LIBEXEC_DIR}")

  if(NOT DEFINED PROJECT_DATADIR)
    set(PROJECT_DATADIR "${datadir}/openbangla-keyboard")
  endif(NOT DEFINED PROJECT_DATADIR)
  add_definitions(-DPROJECT_DATADIR="${PROJECT_DATADIR}")
  add_definitions(-DPKGDATADIR="${PROJECT_DATADIR}")

  include(FindPkgConfig)
  ## Find iBus ##
  find_package(PkgConfig)
  pkg_check_modules(IBUS REQUIRED ibus-1.0)
endif(OS_LINUX)

add_subdirectory(src/engine)
add_subdirectory(src/frontend)
add_subdirectory(src/shared)


set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenSource Bengali input method")
set(CPACK_PACKAGE_VENDOR "Muhammad Mominul Huque")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Muhammad Mominul Huque")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "ibus")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "OpenBanglaKeyboard${CMake_VERSION_MAJOR}")
set(CPACK_STRIP_FILES TRUE)
include(CPack)
