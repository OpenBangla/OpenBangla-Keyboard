include_directories(. ../im ../ )
add_library(Layout
            layout.cpp
            layout.h
            methodphonetic.cpp
            methodphonetic.h
            phoneticparser.cpp
            phoneticparser.h
            regexparser.cpp
            regexparser.h
            autocorrect.cpp
            autocorrect.h
            database.cpp
            database.h
            phoneticsuggestion.cpp
            phoneticsuggestion.h)

target_link_libraries(Layout Qt5::Core Qt5::Sql)
