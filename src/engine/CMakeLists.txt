add_subdirectory(common)

## Platform depended things
if (OS_LINUX)
    ## Platform is Linux
    corrosion_import_crate(
        MANIFEST_PATH riti/Cargo.toml
        CRATE_TYPES staticlib
        CRATES riti
    )

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
endif (OS_LINUX)

if (APPLE)
    ## Platform is macOS
    add_subdirectory(macOS)
endif (APPLE)
