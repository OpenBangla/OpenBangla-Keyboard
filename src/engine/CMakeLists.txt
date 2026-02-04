add_subdirectory(common)

## Platform depended things
if (OS_LINUX)
    ## Platform is Linux

    if (ENABLE_IBUS)
        add_subdirectory(ibus)
    endif()

    if (ENABLE_FCITX)
        add_subdirectory(fcitx)
    endif()

    if(ENABLE_BOTH)
        add_subdirectory(ibus)
        add_subdirectory(fcitx)
    endif()

    ## Include subdirectories
    add_subdirectory(riti)
endif (OS_LINUX)
