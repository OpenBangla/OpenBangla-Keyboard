cmake_minimum_required(VERSION 3.10)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

file(STRINGS "${CMAKE_SOURCE_DIR}/version.txt" VERSION_STRING LIMIT_COUNT 1 REGEX "^.+")
project("OpenBangla-Keyboard" VERSION ${VERSION_STRING})
set(APP_NAME "OpenBangla-Keyboard")

## Set versions
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")

## Set Compiler Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT APPLE)
    enable_language(Rust)
    include(CMakeCargo)
endif()

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

message(STATUS "Qt5 framework version: ${Qt5Core_VERSION}")

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

# For Linux.
if(OS_LINUX)
    ## Additions for Linux ##

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

    option(ENABLE_IBUS "Enable IBus support" OFF)
    option(ENABLE_FCITX "Enable Fcitx support" OFF)

    if((NOT ENABLE_IBUS) AND (NOT ENABLE_FCITX))
        message(FATAL_ERROR "Please set atleast ENABLE_IBUS or ENABLE_FCITX to be ON to build a backend")
    endif()

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

# For Windows
if(WIN32)
    find_package(zstd CONFIG REQUIRED)
    set(PROJECT_DATADIR "$ENV{LOCALAPPDATA}\\OpenBangla" CACHE PATH "Path to Data Directory")
    set(BIN_DIR "${PROJECT_DATADIR}/bin")
    add_definitions(-DPROJECT_DATADIR="${PROJECT_DATADIR}")
    add_definitions(-DBIN_DIR="${BIN_DIR}")

    include(InstallRequiredSystemLibraries)

    # Install data files
    install(FILES
        "${CMAKE_SOURCE_DIR}/data/dictionary.json"
        "${CMAKE_SOURCE_DIR}/data/regex.json"
        "${CMAKE_SOURCE_DIR}/data/suffix.json"
        "${CMAKE_SOURCE_DIR}/src/engine/riti/data/autocorrect.json"
        DESTINATION data)
    install(FILES
        "${CMAKE_SOURCE_DIR}/data/avrophonetic.json"
        "${CMAKE_SOURCE_DIR}/data/Avro_Easy.json"
        "${CMAKE_SOURCE_DIR}/data/Borno.json"
        "${CMAKE_SOURCE_DIR}/data/Munir_Optima.json"
        "${CMAKE_SOURCE_DIR}/data/National_Jatiya.json"
        "${CMAKE_SOURCE_DIR}/data/Probhat.json"
        DESTINATION layouts)
endif()

# For Apple
if(APPLE)
    find_package(zstd CONFIG REQUIRED)

    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)
    set(CMAKE_INSTALL_PREFIX "/")
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
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/data/Readme.txt")
if(APPLE)
    # macOS productbuild requires license files with .txt, .rtf, .rtfd, or .html extension
    configure_file("${CMAKE_SOURCE_DIR}/LICENSE" "${CMAKE_BINARY_DIR}/LICENSE.txt" COPYONLY)
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/LICENSE.txt")
else()
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
endif()
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_PRODUCTBUILD_IDENTITY_NAME "" CACHE STRING "Code signing identity for productbuild")
set(CPACK_PRODUCTBUILD_KEYCHAIN_PATH "" CACHE STRING "Keychain path for code signing")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://openbangla.org/")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
set(CPACK_RPM_PACKAGE_URL "https://openbangla.org/")
set(CPACK_RPM_PACKAGE_RELEASE_DIST ON)
# Prevents CPack from generating file conflicts
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/applications" "/usr/share/metainfo" "/usr/share/pixmaps"
                                                  "/usr/share/icons" "/usr/share/icons/hicolor" 
                                                  "/usr/share/icons/hicolor/1024x1024" "/usr/share/icons/hicolor/1024x1024/apps"
                                                  "/usr/share/icons/hicolor/128x128" "/usr/share/icons/hicolor/128x128/apps"
                                                  "/usr/share/icons/hicolor/16x16" "/usr/share/icons/hicolor/16x16/apps"
                                                  "/usr/share/icons/hicolor/32x32" "/usr/share/icons/hicolor/32x32/apps"
                                                  "/usr/share/icons/hicolor/48x48" "/usr/share/icons/hicolor/48x48/apps"
                                                  "/usr/share/icons/hicolor/512x512" "/usr/share/icons/hicolor/512x512/apps")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${PROJECT_VERSION}-$ENV{DIST}")
# Configure CPack on the basis of which backend we are building for.
if(ENABLE_IBUS)
    ## IBUS
    set(CPACK_PACKAGE_NAME "ibus-openbangla")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "ibus (>= 1.5.1)")
    set(CPACK_RPM_PACKAGE_REQUIRES "qt5-qtbase >= 5.9.0, ibus >= 1.5.1, ibus-libs >= 1.5.1, libzstd >= 1.3.3")
    list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/ibus" "/usr/share/ibus/component")
elseif(ENABLE_FCITX)
    ## FCITX
    set(CPACK_PACKAGE_NAME "fcitx-openbangla")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "fcitx5 (>= 5.0.5)")
    set(CPACK_RPM_PACKAGE_REQUIRES "qt5-qtbase >= 5.9.0, libzstd >= 1.3.3, fcitx5 >= 5.0.5")
    list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/lib64/fcitx5" "/usr/share/fcitx5" "/usr/share/fcitx5/inputmethod" "/usr/share/fcitx5/addon")
elseif(APPLE)
    set(CPACK_PACKAGE_NAME "OpenBangla Keyboard")
    set(CPACK_PACKAGE_VENDOR "OpenBangla")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_FILE_NAME "OpenBangla-Keyboard-${PROJECT_VERSION}")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/")
    set(CPACK_MONOLITHIC_INSTALL OFF)
    set(CPACK_COMPONENTS_ALL IME GUI)
    set(CPACK_COMPONENT_IME_REQUIRED TRUE)
    set(CPACK_COMPONENT_GUI_REQUIRED TRUE)
    set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "FALSE")
    set(CPACK_PRODUCTBUILD_RESOURCES_DIR "${CMAKE_SOURCE_DIR}/data/productbuild")
    set(CPACK_PRODUCTBUILD_BACKGROUND "installer.png")
    set(CPACK_PRODUCTBUILD_BACKGROUND_ALIGNMENT "left")
    set(CPACK_PRODUCTBUILD_BACKGROUND_DARKAQUA "installer.png")
    set(CPACK_PRODUCTBUILD_BACKGROUND_DARKAQUA_ALIGNMENT "left")
elseif(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_NAME "OpenBangla-Keyboard")
    set(CPACK_PACKAGE_FILE_NAME "OpenBangla-Keyboard-${PROJECT_VERSION}-x86_64")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "OpenBangla-Keyboard")
    set(CPACK_PACKAGE_EXECUTABLES "openbangla-gui" "OpenBangla Keyboard")
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_NSIS_DISPLAY_NAME "OpenBangla Keyboard ${PROJECT_VERSION}")
    set(CPACK_NSIS_PACKAGE_NAME "OpenBangla Keyboard")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/src/frontend/openbangla.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/src/frontend/openbangla.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "openbangla-gui.exe")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://openbangla.github.io/")
    set(CPACK_NSIS_HELP_LINK "https://github.com/OpenBangla/OpenBangla-Keyboard")
    set(CPACK_NSIS_MODIFY_PATH OFF)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

    # NSIS install commands: register DLLs and deploy data to %LOCALAPPDATA%
    # Note: NSIS installer is 32-bit, so $SYSDIR points to SysWOW64 (32-bit)
    # and $WINDIR\Sysnative points to the real System32 (64-bit).
    set(_nsis_install_commands
        # Deploy data files to %LOCALAPPDATA%\OpenBangla
        "CreateDirectory \"$LOCALAPPDATA\\OpenBangla\\data\""
        "CreateDirectory \"$LOCALAPPDATA\\OpenBangla\\layouts\""
        "CopyFiles /SILENT \"$INSTDIR\\data\\*.*\" \"$LOCALAPPDATA\\OpenBangla\\data\""
        "CopyFiles /SILENT \"$INSTDIR\\layouts\\*.*\" \"$LOCALAPPDATA\\OpenBangla\\layouts\""
        # Register 64-bit IME DLL (use Sysnative to reach 64-bit regsvr32)
        "ExecWait '\"$WINDIR\\Sysnative\\regsvr32.exe\" /s \"$INSTDIR\\bin\\openbangla.dll\"'"
    )
    set(_nsis_uninstall_commands
        # Unregister 64-bit IME DLL
        "ExecWait '\"$WINDIR\\Sysnative\\regsvr32.exe\" /s /u \"$INSTDIR\\bin\\openbangla.dll\"'"
        # Clean up data directory
        "RMDir /r \"$LOCALAPPDATA\\OpenBangla\""
    )

    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
        list(APPEND _nsis_install_commands
            # Register 32-bit IME DLL (use $SYSDIR which is SysWOW64 in 32-bit NSIS)
            "ExecWait '\"$SYSDIR\\regsvr32.exe\" /s \"$INSTDIR\\bin\\x86\\openbangla.dll\"'"
        )
        list(INSERT _nsis_uninstall_commands 0
            # Unregister 32-bit IME DLL
            "ExecWait '\"$SYSDIR\\regsvr32.exe\" /s /u \"$INSTDIR\\bin\\x86\\openbangla.dll\"'"
        )
    endif()

    string(REPLACE ";" "\n    " _nsis_install_str "${_nsis_install_commands}")
    string(REPLACE ";" "\n    " _nsis_uninstall_str "${_nsis_uninstall_commands}")
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "    ${_nsis_install_str}")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "    ${_nsis_uninstall_str}")
endif()

include(CPack)
