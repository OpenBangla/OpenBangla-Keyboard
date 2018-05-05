set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(3rdParty Downloader.cpp
                     Downloader.h
                     Downloader.ui
                     Updater.cpp
                     Updater.h
                     QSimpleUpdater.cpp
                     QSimpleUpdater.h)

target_link_libraries(3rdParty Qt5::Core Qt5::Widgets Qt5::Network)