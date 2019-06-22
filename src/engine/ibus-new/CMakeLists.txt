include_directories(../riti/include ../../shared ${IBUS_INCLUDE_DIRS})
add_executable(ibus-openbangla main.cpp)
target_link_libraries(ibus-openbangla riti libShared Qt5::Core ${IBUS_LIBRARIES} dl pthread)
