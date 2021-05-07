## Platform depended things
if (OS_LINUX)
    ## Platform is Linux
    add_subdirectory(common)

    if (IBUS_FOUND)
        add_subdirectory(ibus)
    endif()

    if (Fcitx5Core_FOUND)
        add_subdirectory(fcitx)
    endif()
endif (OS_LINUX)

## Include subdirectories
add_subdirectory(riti)
