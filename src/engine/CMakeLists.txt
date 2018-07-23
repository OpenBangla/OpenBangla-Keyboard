## Platform depended things
if (OS_LINUX)
    ## Platform is Linux
    add_subdirectory(ibus)
endif (OS_LINUX)

## Include subdirectories
add_subdirectory(libengine)
