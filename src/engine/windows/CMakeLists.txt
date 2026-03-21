cargo_build(NAME openbangla SHARED)

get_target_property(IME_DLL_PATH openbangla IMPORTED_LOCATION)

# Install the DLL
install(FILES "${IME_DLL_PATH}" DESTINATION "${BIN_DIR}")

# Convenience targets for IME registration (requires elevated privileges)
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
