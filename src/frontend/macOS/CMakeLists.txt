
add_library(macOS STATIC InputSource.swift)
set_target_properties(macOS PROPERTIES Swift_MODULE_NAME "macOS")
target_compile_options(macOS PUBLIC
  "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")

_swift_generate_cxx_header(macOS
  macOS.h
  SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}")
