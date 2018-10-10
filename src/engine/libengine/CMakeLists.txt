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
        AutoCorrect.cpp
        AutoCorrect.h
        Database.cpp
        Database.h
        cachemanager.cpp
        cachemanager.h
        PhoneticSuggestion.cpp
        PhoneticSuggestion.h
        keycode.h)

target_link_libraries(libEngine libShared Qt5::Core)
