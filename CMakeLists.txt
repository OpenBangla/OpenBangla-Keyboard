cmake_minimum_required(VERSION 3.0.2)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

file(STRINGS "${CMAKE_SOURCE_DIR}/version.txt" VERSION_STRING LIMIT_COUNT 1 REGEX "^.+")
project("OpenBangla-Keyboard" VERSION ${VERSION_STRING})
set(APP_NAME "OpenBangla-Keyboard")

## Set versions
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")

## Set Compiler Standard
set(CMAKE_CXX_STANDARD 11)

enable_language(Rust)
include(CMakeCargo)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

# Get system information. WIN32 and APPLE are defined by CMake
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OS_LINUX 1)
endif()

# Set build type to "Release" if user did not specify any build type yet
# Other possible values: Debug, Release, RelWithDebInfo and MinSizeRel
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()

# Sanity check our source directory to make sure that we are not trying to
# generate an in-tree build (it pollutes the source tree with a lot of CMake
# related files).
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed.\nPlease create a build directory and initiate cmake from there.")
endif()

if(NO_UPDATE_CHECK) 
    add_definitions(-DNO_UPDATE_CHECK)
endif()

# For Linux.
if(OS_LINUX)
    ## Additions for Linux ##

    # Generate compile_commands.json to satisfy vscode linux configuration
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    ## Set Directories for installing and storing files
    set(CMAKE_INSTALL_PREFIX "/usr")
    if(NOT DEFINED BIN_DIR)
        set(BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin")
    endif()
    if(NOT DEFINED DATADIR)
        set(DATADIR "${CMAKE_INSTALL_PREFIX}/share")
    endif()
    if(NOT DEFINED LIBEXECDIR)
        set(LIBEXECDIR "${CMAKE_INSTALL_PREFIX}/libexec")
    endif()
    set(PROJECT_DATADIR "${DATADIR}/openbangla-keyboard")
    add_definitions(-DPROJECT_DATADIR="${PROJECT_DATADIR}")
    add_definitions(-DBIN_DIR="${BIN_DIR}")
    add_definitions(-DLIBEXECDIR="${LIBEXECDIR}")

    include(FindPkgConfig)
    find_package(PkgConfig)

    option(ENABLE_IBUS "Enable IBus support" On)
    option(ENABLE_FCITX "Enable Fcitx support" OFF)

    ## Find iBus ##
    if(ENABLE_IBUS)
        pkg_check_modules(IBUS REQUIRED ibus-1.0)
    endif()

    ## Find Fcitx ##
    if(ENABLE_FCITX)
        find_package(Fcitx5Core 5.0.5 REQUIRED) 
    endif()

    ## Find zstd ##
    pkg_check_modules(ZSTD REQUIRED libzstd)
endif()

add_subdirectory(src/engine)
add_subdirectory(src/frontend)
add_subdirectory(src/shared)

# uninstall target
if(NOT TARGET uninstall)
    configure_file(
            "${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
            IMMEDIATE @ONLY)

    add_custom_target(uninstall
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()


set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenSource Bengali input method")
set(CPACK_PACKAGE_VENDOR "OpenBangla Team")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "OpenBangla Team <openbanglateam@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/data/Readme.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://openbangla.github.io/")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
set(CPACK_RPM_PACKAGE_URL "https://openbangla.github.io/")
set(CPACK_RPM_PACKAGE_RELEASE_DIST ON)
# Prevents CPack from generating file conflicts
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/applications")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/metainfo")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/pixmaps")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons/hicolor")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons/hicolor/1024x1024")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons/hicolor/1024x1024/apps")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons/hicolor/128x128")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons/hicolor/128x128/apps")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons/hicolor/16x16")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons/hicolor/16x16/apps")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons/hicolor/32x32")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons/hicolor/32x32/apps")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons/hicolor/48x48")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons/hicolor/48x48/apps")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons/hicolor/512x512")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons/hicolor/512x512/apps")
# Configure CPack on the basis of which backend we are building for.
if(ENABLE_IBUS)
    ## IBUS
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}_${PROJECT_VERSION}-ibus-$ENV{DIST}")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "ibus (>= 1.5.1)")
    set(CPACK_RPM_PACKAGE_REQUIRES "qt5-qtbase >= 5.9.0, ibus >= 1.5.1, ibus-libs >= 1.5.1, libzstd >= 1.3.3")
    list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/ibus" "/usr/share/ibus/component")
else()
    ## FCITX
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}_${PROJECT_VERSION}-fcitx-$ENV{DIST}")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "fcitx5 (>= 5.0.5)")
    set(CPACK_RPM_PACKAGE_REQUIRES "qt5-qtbase >= 5.9.0, libzstd >= 1.3.3, fcitx5 >= 5.0.5")
endif()

include(CPack)
