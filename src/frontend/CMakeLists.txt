include_directories(../shared 3rdParty ${ZSTD_INCLUDE_DIRS})
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

set(LINK_LIBS libShared Qt5::Widgets Qt5::Network ${ZSTD_LIBRARIES})

if(NOT NO_UPDATE_CHECK)
    add_subdirectory(3rdParty)
    list(APPEND LINK_LIBS 3rdParty)
endif()

add_executable(openbangla-gui ${SRC_MAIN} ${SRC_GUI})
target_link_libraries(openbangla-gui ${LINK_LIBS})

install(TARGETS openbangla-gui
        RUNTIME DESTINATION ${LIBEXECDIR}/openbangla-keyboard)
