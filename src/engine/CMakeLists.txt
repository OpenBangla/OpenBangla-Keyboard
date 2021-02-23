## Platform depended things
if (OS_LINUX)
    add_subdirectory(common)
    ## Platform is Linux
    if (ENABLE_IBUS)
        add_subdirectory(ibus)
    endif()

    if (ENABLE_FCITX)
        add_subdirectory(fcitx)
    endif()
endif (OS_LINUX)

## Include subdirectories
add_subdirectory(riti)
