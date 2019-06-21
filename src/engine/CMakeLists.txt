## Platform depended things
if (OS_LINUX)
    ## Platform is Linux
    #add_subdirectory(ibus)
    add_subdirectory(ibus-new)
endif (OS_LINUX)

## Include subdirectories
#add_subdirectory(libengine)
add_subdirectory(riti)
