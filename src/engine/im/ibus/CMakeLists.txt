include_directories(. ../../ ../../layout ${IBUS_INCLUDE_DIRS})
add_library(IM_iBus ibus_engine.cpp
                    ibus_keycode.h)

target_link_libraries(IM_iBus Qt5::Core Qt5::Sql)
