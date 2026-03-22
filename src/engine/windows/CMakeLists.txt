cargo_build(NAME openbangla SHARED)

get_target_property(IME_DLL_PATH openbangla IMPORTED_LOCATION)

# Install the DLL
install(FILES "${IME_DLL_PATH}" DESTINATION "${BIN_DIR}")

# Build 32-bit DLL for x86_64 to support WOW64 (32-bit) applications
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
    cargo_build(NAME openbangla_x86 SHARED TARGET i686-pc-windows-msvc CRATE_NAME openbangla)

    get_target_property(IME_DLL_X86_PATH openbangla_x86 IMPORTED_LOCATION)
    install(FILES "${IME_DLL_X86_PATH}" DESTINATION "${BIN_DIR}/x86")
endif()

# Convenience targets for IME registration (requires elevated privileges)
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
    add_custom_target(register-ime
        COMMAND regsvr32 /s "${IME_DLL_PATH}"
        COMMAND regsvr32 /s "${IME_DLL_X86_PATH}"
        DEPENDS openbangla openbangla_x86
        COMMENT "Registering OpenBangla IME (64-bit and 32-bit)..."
        VERBATIM
    )

    add_custom_target(unregister-ime
        COMMAND regsvr32 /s /u "${IME_DLL_PATH}"
        COMMAND regsvr32 /s /u "${IME_DLL_X86_PATH}"
        COMMENT "Unregistering OpenBangla IME (64-bit and 32-bit)..."
        VERBATIM
    )
else()
    add_custom_target(register-ime
        COMMAND regsvr32 /s "${IME_DLL_PATH}"
        DEPENDS openbangla
        COMMENT "Registering OpenBangla IME..."
        VERBATIM
    )

    add_custom_target(unregister-ime
        COMMAND regsvr32 /s /u "${IME_DLL_PATH}"
        COMMENT "Unregistering OpenBangla IME..."
        VERBATIM
    )
endif()
