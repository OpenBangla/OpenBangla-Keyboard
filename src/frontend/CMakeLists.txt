include_directories(../shared 3rdParty)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRC_GUI TopBar.cpp
        AboutDialog.cpp
        AboutFile.cpp
        LayoutViewer.cpp
        SettingsDialog.cpp
        AutoCorrectDialog.cpp
        images.qrc)

set(SRC_MAIN main.cpp
        Layout.cpp
        LayoutConverter.cpp
        LayoutConverter.h
        SingleInstance.cpp
        AvroPhonetic.cpp)

set(LINK_LIBS libShared Qt5::Widgets Qt5::Network)

if(OS_LINUX)
    list(APPEND LINK_LIBS ${ZSTD_LIBRARIES})
endif(OS_LINUX)

if(WIN32)
    list(APPEND LINK_LIBS zstd::libzstd)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif(WIN32)

if(APPLE)
    list(APPEND LINK_LIBS zstd::libzstd)
    list(APPEND LINK_LIBS "-framework Foundation")
endif(APPLE)

if(APPLE)
    set(APP_ICON "${CMAKE_SOURCE_DIR}/data/openbangla.icns")

    set(APP_DATA
        "${CMAKE_SOURCE_DIR}/data/autocorrect.json"
    )

    set(LAYOUT_DATA
        "${CMAKE_SOURCE_DIR}/data/avrophonetic.json"
        "${CMAKE_SOURCE_DIR}/data/Avro_Easy.json"
        "${CMAKE_SOURCE_DIR}/data/Borno.json"
        "${CMAKE_SOURCE_DIR}/data/Munir_Optima.json"
        "${CMAKE_SOURCE_DIR}/data/National_Jatiya.json"
        "${CMAKE_SOURCE_DIR}/data/Probhat.json"
    )

    set_source_files_properties(
        ${APP_ICON}
        PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )

    set_source_files_properties(
            ${APP_DATA}
            PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources/data"
    )

    set_source_files_properties(
            ${LAYOUT_DATA}
            PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources/layouts"
    )

    add_executable(openbangla-gui MACOSX_BUNDLE ${SRC_MAIN} ${SRC_GUI} ${APP_ICON} ${APP_DATA} ${LAYOUT_DATA})

    set_target_properties(openbangla-gui PROPERTIES
        OUTPUT_NAME "OpenBangla Keyboard"
        MACOSX_BUNDLE_ICON_FILE openbangla.icns
        MACOSX_BUNDLE_BUNDLE_NAME "OpenBangla Keyboard"
        MACOSX_BUNDLE_INFO_STRING "OpenBangla Keyboard"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.openbangla.keyboard"
        MACOSX_BUNDLE_COPYRIGHT "© 2025 OpenBangla"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    )

    get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
    
        add_custom_command(TARGET openbangla-gui POST_BUILD
            COMMAND "${MACDEPLOYQT_EXECUTABLE}"
                    "$<TARGET_BUNDLE_DIR:openbangla-gui>"
                    -always-overwrite
            COMMAND codesign --force --deep --sign -
                    "$<TARGET_BUNDLE_DIR:openbangla-gui>"
            COMMENT "Deploying Qt and signing app"
            VERBATIM
        )
else()
    add_executable(openbangla-gui ${SRC_MAIN} ${SRC_GUI})
endif()

target_link_libraries(openbangla-gui ${LINK_LIBS})

if(WIN32)
    get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    add_custom_command(TARGET openbangla-gui POST_BUILD
    COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:openbangla-gui>
    COMMENT "Deploying Qt..."
    VERBATIM
    )
endif()

if(OS_LINUX)
    install(TARGETS openbangla-gui
        RUNTIME DESTINATION ${BIN_DIR})
endif()

if(APPLE)
    install(TARGETS openbangla-gui
        BUNDLE DESTINATION "Applications")
endif()
