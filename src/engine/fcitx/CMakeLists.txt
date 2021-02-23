include("${FCITX_INSTALL_CMAKECONFIG_DIR}/Fcitx5Utils/Fcitx5CompilerSettings.cmake")

find_package(Pthread REQUIRED)
find_package(DL REQUIRED)

add_library(openbangla MODULE openbangla.cpp)
target_link_libraries(openbangla Fcitx5::Core riti Pthread::Pthread DL::DL)
target_include_directories(openbangla PRIVATE ../riti/include)
set_target_properties(openbangla PROPERTIES PREFIX "")
install(TARGETS openbangla DESTINATION "${FCITX_INSTALL_LIBDIR}/fcitx5")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/openbangla.conf" DESTINATION "${FCITX_INSTALL_PKGDATADIR}/inputmethod")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/openbangla-addon.conf" RENAME openbangla.conf DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon")
