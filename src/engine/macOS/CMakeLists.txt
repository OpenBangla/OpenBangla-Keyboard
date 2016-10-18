set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY})
set(NAME OpenBangla-Engine)
set(MODULE OpenBangla_Engine)
set(TARGET OpenBangla)
set(SOURCES main.swift Keycodes.swift)

list(APPEND ADDITIONAL_ARGS -llibEngine)
list(APPEND ADDITIONAL_ARGS -I ${CMAKE_CURRENT_SOURCE_DIR}/../libengine/c-api)
list(APPEND ADDITIONAL_ARGS -F ${FRAMEWORKS_PATH})

list(APPEND ARGS -module-name ${MODULE})
list(APPEND ARGS -incremental -emit-dependencies -emit-module)
list(APPEND ARGS -emit-module-path ${MODULE}.swiftmodule)

set(FILEMAP ${BUILD_DIR}/output-file-map.json)
set(OUTPUT_FILE_MAP ${FILEMAP})

# Remove old file and start writing new one.
file(REMOVE ${FILEMAP})
file(APPEND ${FILEMAP} "{\n")
foreach(source ${SOURCES})
  file(APPEND ${FILEMAP} "\"${CMAKE_CURRENT_SOURCE_DIR}/${source}\": {\n")
  file(APPEND ${FILEMAP} "\"dependencies\": \"${BUILD_DIR}/${source}.d\",\n")
  set(OBJECT ${BUILD_DIR}/${source}.o)
  list(APPEND OUTPUTS ${OBJECT})
  file(APPEND ${FILEMAP} "\"object\": \"${OBJECT}\",\n")
  file(APPEND ${FILEMAP} "\"swiftmodule\": \"${BUILD_DIR}/${source}~partial.swiftmodule\",\n")
  file(APPEND ${FILEMAP} "\"swift-dependencies\": \"${BUILD_DIR}/${source}.swiftdeps\"\n},\n")
endforeach()
file(APPEND ${FILEMAP} "\"\": {\n")
file(APPEND ${FILEMAP} "\"swift-dependencies\": \"${BUILD_DIR}/master.swiftdeps\"\n")
file(APPEND ${FILEMAP} "}\n")
file(APPEND ${FILEMAP} "}")

list(APPEND ARGS -output-file-map ${OUTPUT_FILE_MAP})
#list(APPEND ARGS -parse-as-library)
list(APPEND ARGS -c)

foreach(source ${SOURCES})
  list(APPEND ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${source})
endforeach()

## TODO: Check Build Types
list(APPEND ARGS -Onone -g)

foreach(arg ${ADDITIONAL_ARGS})
  list(APPEND ARGS ${arg})
endforeach()

list(APPEND ARGS -sdk ${CMAKE_OSX_SYSROOT})

## Compile
add_custom_command(
  OUTPUT    ${OUTPUTS}
  COMMAND   swiftc
  ARGS      ${ARGS}
  DEPENDS   ${SOURCES}
)

## Link & Create Executable
set(EXE_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${NAME})

list(APPEND EXE_ARGS -sdk ${CMAKE_OSX_SYSROOT})

#list(APPEND EXE_ARGS -module-name ${NAME})
list(APPEND EXE_ARGS -o ${EXE_OUTPUT})
list(APPEND EXE_ARGS -emit-executable ${OUTPUTS})

foreach(arg ${ADDITIONAL_ARGS})
  list(APPEND EXE_ARGS ${arg})
endforeach()

list(APPEND EXE_ARGS -L $<TARGET_FILE_DIR:libEngine>)

add_custom_command(
  OUTPUT    ${EXE_OUTPUT}
  COMMAND   swiftc
  ARGS      ${EXE_ARGS}
  DEPENDS   ${OUTPUTS}
)

## Add the target
add_custom_target(${TARGET} ALL DEPENDS libEngine ${EXE_OUTPUT} ${SOURCES})
