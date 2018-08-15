## Platform depended things
if (OS_LINUX)
    ## Platform is Linux
    add_subdirectory(ibus)
endif (OS_LINUX)

if(WIN32)
    ## Platform is Windows
    add_subdirectory(windows)
endif(WIN32)

## Include subdirectories
add_subdirectory(libengine)
