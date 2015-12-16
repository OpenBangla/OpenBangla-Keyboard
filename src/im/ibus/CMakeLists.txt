INCLUDE_DIRECTORIES(. ../../ ../../layout ${IBUS_INCLUDE_DIRS})
ADD_LIBRARY(IM_iBus ibus_engine.cpp
                    ibus_keycode.h)
