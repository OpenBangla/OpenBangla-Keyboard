include_directories(. ../im ../ )
add_library(Layout
            layout.cpp
            layout.h
            methodphonetic.cpp
            phoneticparser.cpp
            phoneticparser.h
            regexparser.cpp
            regexparser.h
            database.cpp
            database.h
            phoneticsuggestion.cpp
            phoneticsuggestion.h)

target_link_libraries(Layout Qt5::Core Qt5::Sql)
