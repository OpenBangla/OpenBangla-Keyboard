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

if(OS_LINUX)
  add_executable(OpenBangla-Keyboard ${SRC_MAIN} ${SRC_GUI})
elseif(APPLE)
  add_executable(OpenBangla-Keyboard MACOSX_BUNDLE ${SRC_MAIN} ${SRC_GUI})
endif()

target_link_libraries(OpenBangla-Keyboard libEngine libShared 3rdParty Qt5::Widgets Qt5::Network)

if(NOT APPLE)
install(TARGETS OpenBangla-Keyboard
        RUNTIME DESTINATION ${PROJECT_DATADIR})
endif()
