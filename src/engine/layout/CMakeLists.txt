include_directories(. ../im ../ )
add_library(Layout
            Layout.cpp
            Layout.h
            methodphonetic.cpp
            methodphonetic.h
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
            phoneticsuggestion.h)

target_link_libraries(Layout Qt5::Core Qt5::Sql)
