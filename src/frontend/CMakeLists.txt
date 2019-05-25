include_directories(../shared ../engine/libengine 3rdParty)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(3rdParty)

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
        SingleInstance.cpp)

add_executable(openbangla-gui ${SRC_MAIN} ${SRC_GUI})
target_link_libraries(openbangla-gui libEngine libShared 3rdParty Qt5::Widgets Qt5::Network)

install(TARGETS openbangla-gui
        RUNTIME DESTINATION ${PROJECT_DATADIR})
