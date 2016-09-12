include_directories(. ../ ../../shared)

set(Sources Layout.cpp
            Layout.h
            MethodPhonetic.cpp
            MethodPhonetic.h
            MethodFixedLayoutModern.cpp
            phoneticparser.cpp
            phoneticparser.h
            FixedLayoutParser.cpp
            regexparser.cpp
            regexparser.h
            autocorrect.cpp
            autocorrect.h
            database.cpp
            database.h
            cachemanager.cpp
            cachemanager.h
            phoneticsuggestion.cpp
            phoneticsuggestion.h
            keycode.h)

if(APPLE)
  list(APPEND Sources CEngine.cpp)
endif()

add_library(libEngine ${Sources})

if(APPLE)
  target_compile_options(libEngine PUBLIC -fmodules -fmodule-map-file=${CMAKE_SOURCE_DIR}/src/engine/libengine/module.modulemap -fmodules-cache-path=${CMAKE_BINARY_DIR}/cache )
endif()

target_link_libraries(libEngine libShared Qt5::Core Qt5::Sql)
