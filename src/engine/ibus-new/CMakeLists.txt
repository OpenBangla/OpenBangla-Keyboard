include_directories(../riti/include ../../shared)
add_executable(ibus-openbangla main.cpp)
target_link_libraries(ibus-openbangla riti libShared Qt5::Core dl pthread)
