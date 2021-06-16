## Platform depended things
if (OS_LINUX)
    ## Platform is Linux
    add_subdirectory(common)

    if (ENABLE_IBUS)
        add_subdirectory(ibus)
    endif()

    if (ENABLE_FCITX)
        add_subdirectory(fcitx)
    endif()
endif (OS_LINUX)

## Include subdirectories
add_subdirectory(riti)
