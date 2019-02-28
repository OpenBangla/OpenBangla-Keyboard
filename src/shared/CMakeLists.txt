#include_directories(.)

add_library(libShared
        Settings.cpp
        FileSystem.cpp)

target_link_libraries(libShared Qt5::Core)
