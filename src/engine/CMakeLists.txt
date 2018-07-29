## Platform depended things
if (OS_LINUX)
    ## Platform is Linux
    add_subdirectory(ibus)
endif (OS_LINUX)

if(APPLE)
  ## Platform is macOS
  add_subdirectory(macOS)
endif()

## Include subdirectories
add_subdirectory(libengine)
