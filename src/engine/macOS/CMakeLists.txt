set(MACOS_ENGINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(XCODE_PROJECT "${MACOS_ENGINE_DIR}/OpenBangla.xcodeproj")
set(XCODE_SCHEME "OpenBangla")

# Map CMake build type to Xcode configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(XCODE_CONFIGURATION "Debug")
else()
    set(XCODE_CONFIGURATION "Release")
endif()

# Output paths
set(XCODE_DERIVED_DATA "${CMAKE_CURRENT_BINARY_DIR}/build")
set(BUILT_APP_PATH "${XCODE_DERIVED_DATA}/Build/Products/${XCODE_CONFIGURATION}/OpenBangla.app")

# Build the Input Method using xcodebuild
add_custom_target(openbangla-engine ALL
    COMMAND xcodebuild
        -project "${XCODE_PROJECT}"
        -scheme "${XCODE_SCHEME}"
        -configuration "${XCODE_CONFIGURATION}"
        -derivedDataPath "${XCODE_DERIVED_DATA}"
        MARKETING_VERSION="${PROJECT_VERSION}"
        CURRENT_PROJECT_VERSION="${PROJECT_VERSION}"
    COMMENT "Building OpenBangla Input Method with xcodebuild (${XCODE_CONFIGURATION})"
    WORKING_DIRECTORY "${MACOS_ENGINE_DIR}"
    VERBATIM
)

# Copy the built app to a known location for installation
set(STAGED_APP_PATH "${CMAKE_CURRENT_BINARY_DIR}/staged/OpenBangla.app")
add_custom_command(TARGET openbangla-engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${STAGED_APP_PATH}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${BUILT_APP_PATH}"
        "${STAGED_APP_PATH}"
    COMMENT "Staging OpenBangla.app for installation"
    VERBATIM
)

# Clean target for xcode build artifacts
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
    "${XCODE_DERIVED_DATA}"
    "${CMAKE_CURRENT_BINARY_DIR}/staged"
)

install(DIRECTORY "${STAGED_APP_PATH}"
    DESTINATION "/Library/Input Methods"
    USE_SOURCE_PERMISSIONS
)
