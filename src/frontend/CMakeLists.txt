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
    list(APPEND SRC_MAIN app.rc)
    list(APPEND LINK_LIBS zstd::libzstd)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif(WIN32)

add_executable(openbangla-gui ${SRC_MAIN} ${SRC_GUI})
target_link_libraries(openbangla-gui ${LINK_LIBS})

install(TARGETS openbangla-gui
        RUNTIME DESTINATION ${BIN_DIR})
