set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRC_GUI TopBar.cpp
            AboutDialog.cpp
            AboutFile.cpp
            LayoutViewer.cpp
            SettingsDialog.cpp
            images.qrc)

set(SRC_MAIN main.cpp
             Layout.cpp
             Settings.cpp
             LayoutConverter.cpp
             LayoutConverter.h
             SingleInstance.cpp)

add_executable(OpenBangla-Keyboard ${SRC_MAIN} ${SRC_GUI})
target_link_libraries(OpenBangla-Keyboard Qt5::Widgets Qt5::Xml Qt5::Network)

install(TARGETS OpenBangla-Keyboard
RUNTIME DESTINATION  ${PROJECT_DATADIR})
