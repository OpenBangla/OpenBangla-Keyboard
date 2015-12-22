include_directories(. ../../ ../../layout ${IBUS_INCLUDE_DIRS})
add_library(IM_iBus ibus_engine.cpp
                    ibus_keycode.h)
