include_directories(. ../ ../libengine ${IBUS_INCLUDE_DIRS})
link_directories(${CMAKE_CURRENT_BINARY_DIR}/libengine)

set(iBus ibus_engine.cpp
        ibus_keycode.h)

add_executable(OpenBangla-Engine ${iBus})
target_link_libraries(OpenBangla-Engine libEngine Qt5::Core ${IBUS_LIBRARIES})

## Configure the files
configure_file(${CMAKE_SOURCE_DIR}/data/openbangla.xml.in ${CMAKE_BINARY_DIR}/data/openbangla.xml)
configure_file(${CMAKE_SOURCE_DIR}/data/openbangla-keyboard.desktop.in ${CMAKE_BINARY_DIR}/data/openbangla-keyboard.desktop)

## Install things
install(FILES ${CMAKE_SOURCE_DIR}/data/32.png
        DESTINATION ${PROJECT_DATADIR}/icons
        RENAME OpenBangla-Keyboard.png)

install(FILES ${CMAKE_SOURCE_DIR}/data/1024.png
        DESTINATION ${datadir}/pixmaps/
        RENAME openbangla-keyboard.png)

install(FILES ${CMAKE_SOURCE_DIR}/data/16.png
        DESTINATION ${datadir}/icons/hicolor/16x16/apps/
        RENAME openbangla-keyboard.png)

install(FILES ${CMAKE_SOURCE_DIR}/data/32.png
        DESTINATION ${datadir}/icons/hicolor/32x32/apps/
        RENAME openbangla-keyboard.png)

install(FILES ${CMAKE_SOURCE_DIR}/data/48.png
        DESTINATION ${datadir}/icons/hicolor/48x48/apps/
        RENAME openbangla-keyboard.png)

install(FILES ${CMAKE_SOURCE_DIR}/data/128.png
        DESTINATION ${datadir}/icons/hicolor/128x128/apps/
        RENAME openbangla-keyboard.png)

install(FILES ${CMAKE_SOURCE_DIR}/data/512.png
        DESTINATION ${datadir}/icons/hicolor/512x512/apps/
        RENAME openbangla-keyboard.png)

install(FILES ${CMAKE_SOURCE_DIR}/data/1024.png
        DESTINATION ${datadir}/icons/hicolor/1024x1024/apps/
        RENAME openbangla-keyboard.png)

install(FILES ${CMAKE_SOURCE_DIR}/data/avrophonetic.json
        DESTINATION ${PROJECT_DATADIR}/layouts)

install(FILES ${CMAKE_SOURCE_DIR}/data/Bornona.json
        DESTINATION ${PROJECT_DATADIR}/layouts)

install(FILES ${CMAKE_SOURCE_DIR}/data/Avro_Easy.json
        DESTINATION ${PROJECT_DATADIR}/layouts)

install(FILES ${CMAKE_SOURCE_DIR}/data/Munir_Optima_uni_.json
        DESTINATION ${PROJECT_DATADIR}/layouts)

install(FILES ${CMAKE_SOURCE_DIR}/data/National_Jatiya.json
        DESTINATION ${PROJECT_DATADIR}/layouts)

install(FILES ${CMAKE_SOURCE_DIR}/data/Probhat.json
        DESTINATION ${PROJECT_DATADIR}/layouts)

install(FILES ${CMAKE_SOURCE_DIR}/data/autocorrect.json
        DESTINATION ${PROJECT_DATADIR}/data)

install(FILES ${CMAKE_SOURCE_DIR}/data/dictionary.json
        DESTINATION ${PROJECT_DATADIR}/data)

install(FILES ${CMAKE_SOURCE_DIR}/data/suffix.json
        DESTINATION ${PROJECT_DATADIR}/data)

install(FILES ${CMAKE_SOURCE_DIR}/data/regex.json
        DESTINATION ${PROJECT_DATADIR}/data)

install(FILES ${CMAKE_BINARY_DIR}/data/openbangla.xml
        DESTINATION "${datadir}/ibus/component")

install(FILES ${CMAKE_BINARY_DIR}/data/openbangla-keyboard.desktop
        DESTINATION "${datadir}/applications")

install(FILES ${CMAKE_SOURCE_DIR}/data/io.github.openbangla.keyboard.metainfo.xml
        DESTINATION ${datadir}/metainfo)

install(TARGETS OpenBangla-Engine
        RUNTIME DESTINATION ${PROJECT_DATADIR})
