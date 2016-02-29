set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(OpenBangla-Keyboard main.cpp Layout.cpp Settings.cpp TopBar.cpp AboutFile.cpp LayoutViewer.cpp images.qrc)
target_link_libraries(OpenBangla-Keyboard Qt5::Widgets)
