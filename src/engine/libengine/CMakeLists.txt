include_directories(. ../ )
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
            keycode.h
            Settings.cpp
            Settings.h)

target_link_libraries(libEngine Qt5::Core Qt5::Sql)
