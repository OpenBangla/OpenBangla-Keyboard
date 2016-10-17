cmake_minimum_required(VERSION 3.0.2)

project(OpenBangla-Keyboard VERSION 1.0.0)
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

# For Linux.
if(OS_LINUX)
  # Include our Linux specific additions
  include(cmake/CMakeLinux.txt)

  include(FindPkgConfig)
  ## Find iBus ##
  find_package(PkgConfig)
  pkg_check_modules(IBUS REQUIRED ibus-1.0)
endif(OS_LINUX)

if(APPLE)
  ## Find Swift compiler
  execute_process(
    COMMAND xcrun --find swiftc
    OUTPUT_VARIABLE SWIFTC_EXECUTABLE
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(SWIFTC_FOUND TRUE)
  message(STATUS "Found swiftc: ${SWIFTC_EXECUTABLE}")

  # Get the SDK path for OSX.
  execute_process(
    COMMAND xcrun --sdk macosx --show-sdk-path
    OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  ## Get the Frameworks path
  execute_process(
    COMMAND xcrun --sdk macosx --show-sdk-platform-path
    OUTPUT_VARIABLE PLATFORM_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  set(FRAMEWORKS_PATH ${PLATFORM_PATH}/Developer/Library/Frameworks)

  include(cmake/CMakeMacOS.txt)
endif()

add_subdirectory(src/engine)
add_subdirectory(src/frontend)
add_subdirectory(src/shared)


set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenSource Bengali input method")
set(CPACK_PACKAGE_VENDOR "Muhammad Mominul Huque")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Muhammad Mominul Huque")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "ibus,libibus-1.0-dev,libqt5core5a,libqt5widgets5,libqt5sql5,libqt5sql5-sqlite,libqt5xml5,libqt5network5")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "OpenBanglaKeyboard${CMake_VERSION_MAJOR}")
#if(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
#  set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
#  set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
#  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
#  set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
#  set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
#  set(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
#  set(CPACK_NSIS_MODIFY_PATH ON)
#else(WIN32 AND NOT UNIX)
#  set(CPACK_STRIP_FILES "bin/OpenBanglaKeyboard")
#  set(CPACK_SOURCE_STRIP_FILES "")
#endif(WIN32 AND NOT UNIX)
#set(CPACK_PACKAGE_EXECUTABLES "OpenBanglaKeyboard" "OpenBangla Keyboard")
include(CPack)
