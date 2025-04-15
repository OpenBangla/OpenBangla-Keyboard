include_directories(../shared ${ZSTD_INCLUDE_DIRS})
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

add_executable(openbangla-gui ${SRC_MAIN} ${SRC_GUI})
target_link_libraries(openbangla-gui ${LINK_LIBS})

install(TARGETS openbangla-gui
        RUNTIME DESTINATION ${BIN_DIR})
