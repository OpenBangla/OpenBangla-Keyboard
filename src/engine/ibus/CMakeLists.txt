include_directories(. ../ ../libengine ../libengine/c-api ${IBUS_INCLUDE_DIRS})
link_directories(${CMAKE_CURRENT_BINARY_DIR}/libengine)

set(iBus ibus_engine.cpp
         ibus_keycode.h)

add_executable(OpenBangla-Engine ${iBus})
target_link_libraries(OpenBangla-Engine libEngine Qt5::Core ${IBUS_LIBRARIES})

install(TARGETS OpenBangla-Engine
        RUNTIME DESTINATION ${LIBEXEC_DIR}
        )
