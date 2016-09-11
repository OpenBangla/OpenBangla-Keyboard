include_directories(. ../ ../../shared)
add_library(libEngine
            Layout.cpp
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

target_link_libraries(libEngine libShared Qt5::Core Qt5::Sql)
