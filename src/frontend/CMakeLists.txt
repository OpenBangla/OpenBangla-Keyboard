# include_directories(../shared 3rdParty ${ZSTD_INCLUDE_DIRS})
include_directories(../shared 3rdParty)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRC_GUI TopBar.cpp
        AboutDialog.cpp
        AboutFile.cpp
        LayoutViewer.cpp
        SettingsDialog.cpp
        AutoCorrectDialog.cpp
        images.qrc)

set(SRC_MAIN main.cpp
        Layout.cpp
        LayoutConverter.cpp
        LayoutConverter.h
        SingleInstance.cpp
        AvroPhonetic.cpp)

set(LINK_LIBS libShared Qt5::Widgets Qt5::Network)

if(OS_LINUX)
    list(APPEND LINK_LIBS ${ZSTD_LIBRARIES})
endif(OS_LINUX)

if(WIN32)
    list(APPEND LINK_LIBS zstd::libzstd)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif(WIN32)

if(NOT NO_UPDATE_CHECK)
    add_subdirectory(3rdParty)
    list(APPEND LINK_LIBS 3rdParty)
endif()

add_executable(openbangla-gui ${SRC_MAIN} ${SRC_GUI})
target_link_libraries(openbangla-gui ${LINK_LIBS})

if(WIN32)
    get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    add_custom_command(TARGET openbangla-gui POST_BUILD
    COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:openbangla-gui>
    COMMENT "Deploying Qt..."
    VERBATIM
    )
endif()

install(TARGETS openbangla-gui
        RUNTIME DESTINATION ${BIN_DIR})
