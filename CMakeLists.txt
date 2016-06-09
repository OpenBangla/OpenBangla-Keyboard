cmake_minimum_required(VERSION 3.0.2)

project(OpenBangla-Keyboard)
set(APP_NAME "OpenBangla-Keyboard")

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

# For Linux.
if(OS_LINUX)
  # Include our Linux specific additions
  include(cmake/CMakeLinux.txt)

  include(FindPkgConfig)
  ## Find iBus ##
  find_package(PkgConfig)
  pkg_check_modules(IBUS REQUIRED ibus-1.0)
endif(OS_LINUX)

add_subdirectory(src/engine)
add_subdirectory(src/frontend)


set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenSource Bengali input method")
set(CPACK_PACKAGE_VENDOR "Muhammad Mominul Huque")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Muhammad Mominul Huque")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "OpenBanglaKeyboard${CMake_VERSION_MAJOR}")
if(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
  set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
  set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
  set(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
  set(CPACK_NSIS_MODIFY_PATH ON)
else(WIN32 AND NOT UNIX)
  set(CPACK_STRIP_FILES "bin/OpenBanglaKeyboard")
  set(CPACK_SOURCE_STRIP_FILES "")
endif(WIN32 AND NOT UNIX)
set(CPACK_PACKAGE_EXECUTABLES "OpenBanglaKeyboard" "OpenBangla Keyboard")
include(CPack)
