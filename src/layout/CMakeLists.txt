include_directories(. ../im ../ )
add_library(Layout
            layout.cpp
            layout.h
            methodphonetic.cpp
            phoneticparser.cpp
            phoneticparser.h
            regexparser.cpp
            regexparser.h
            phoneticsuggestion.cpp
            phoneticsuggestion.h)
