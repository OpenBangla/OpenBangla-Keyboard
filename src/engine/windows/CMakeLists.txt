corrosion_import_crate(
    MANIFEST_PATH Cargo.toml
    CRATE_TYPES cdylib
    CRATES openbangla
)

# Install the DLL (relative path for CPack compatibility)
install(FILES $<TARGET_FILE:openbangla-shared> DESTINATION bin)

# Build 32-bit DLL for x86_64 to support WOW64 (32-bit) applications
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
    set(_x86_target_dir "${CMAKE_BINARY_DIR}/cargo/x86")
    set(_x86_cargo_profile "$<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:>>>:--release>")
    set(_x86_build_type_dir "$<IF:$<OR:$<CONFIG:Debug>,$<CONFIG:>>,debug,release>")

    add_custom_target(openbangla_x86 ALL
        COMMAND
            ${CMAKE_COMMAND} -E env
            "CARGO_BUILD_RUSTC=${_CORROSION_RUSTC}"
            ${Rust_CARGO_CACHED} build
            ${_x86_cargo_profile}
            --target i686-pc-windows-msvc
            --manifest-path "${CMAKE_CURRENT_SOURCE_DIR}/Cargo.toml"
            --target-dir "${_x86_target_dir}"
        COMMENT "Building 32-bit OpenBangla IME DLL..."
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
    # Serialize after the 64-bit build to avoid concurrent cargo/rustup races
    add_dependencies(openbangla_x86 cargo-build_openbangla)

    set(IME_DLL_X86_PATH "${_x86_target_dir}/i686-pc-windows-msvc/${_x86_build_type_dir}/openbangla.dll")
    install(FILES "${IME_DLL_X86_PATH}" DESTINATION bin/x86)
endif()

# Convenience targets for IME registration (requires elevated privileges)
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
    add_custom_target(register-ime
        COMMAND regsvr32 /s $<TARGET_FILE:openbangla-shared>
        COMMAND regsvr32 /s "${IME_DLL_X86_PATH}"
        DEPENDS openbangla openbangla_x86
        COMMENT "Registering OpenBangla IME (64-bit and 32-bit)..."
        VERBATIM
    )

    add_custom_target(unregister-ime
        COMMAND regsvr32 /s /u $<TARGET_FILE:openbangla-shared>
        COMMAND regsvr32 /s /u "${IME_DLL_X86_PATH}"
        COMMENT "Unregistering OpenBangla IME (64-bit and 32-bit)..."
        VERBATIM
    )
else()
    add_custom_target(register-ime
        COMMAND regsvr32 /s $<TARGET_FILE:openbangla-shared>
        DEPENDS openbangla
        COMMENT "Registering OpenBangla IME..."
        VERBATIM
    )

    add_custom_target(unregister-ime
        COMMAND regsvr32 /s /u $<TARGET_FILE:openbangla-shared>
        COMMENT "Unregistering OpenBangla IME..."
        VERBATIM
    )
endif()
