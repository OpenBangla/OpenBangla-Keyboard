include_directories(../riti/include ../../shared ${IBUS_INCLUDE_DIRS})
add_executable(ibus-engine-openbangla main.cpp)
target_link_libraries(ibus-engine-openbangla riti libShared Qt5::Core ${IBUS_LIBRARIES} dl pthread)

## Configure the files
configure_file(${CMAKE_SOURCE_DIR}/data/openbangla.xml.in ${CMAKE_BINARY_DIR}/data/openbangla.xml)

install(FILES ${CMAKE_BINARY_DIR}/data/openbangla.xml
        DESTINATION "${DATADIR}/ibus/component")

install(TARGETS ibus-engine-openbangla
        RUNTIME DESTINATION ${LIBEXECDIR})
