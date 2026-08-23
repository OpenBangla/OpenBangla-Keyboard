# Bundle the MSYS2 / Clang runtime and Qt third-party dependency DLLs that
# windeployqt does not copy (libc++, libunwind, libwinpthread, and Qt's own
# deps such as libpng, pcre2, harfbuzz, freetype, zlib, ...). Without these the
# installed executable fails to start outside an MSYS2 shell.
#
# Invoked via install(SCRIPT), so CMAKE_INSTALL_PREFIX is available here.
# Runs inside the MSYS2 shell, where ldd, awk and cygpath are present.

find_program(_obk_ldd ldd)
find_program(_obk_cygpath cygpath)
if(NOT _obk_ldd OR NOT _obk_cygpath)
  message(WARNING "ldd/cygpath not found; skipping MSYS2 runtime DLL bundling")
  return()
endif()

set(_obk_exe "${CMAKE_INSTALL_PREFIX}/openbangla-gui.exe")

execute_process(
  COMMAND "${_obk_ldd}" "${_obk_exe}"
  OUTPUT_VARIABLE _obk_ldd_out
  RESULT_VARIABLE _obk_ldd_res)
if(NOT _obk_ldd_res EQUAL 0)
  message(WARNING "ldd failed on ${_obk_exe}; skipping runtime DLL bundling")
  return()
endif()

string(REPLACE "\n" ";" _obk_lines "${_obk_ldd_out}")
foreach(_obk_line IN LISTS _obk_lines)
  # ldd lines look like: "<name> => <unix-path> (0x...)"
  if(NOT _obk_line MATCHES "=> (/[^ ]+) \\(")
    continue()
  endif()
  set(_obk_dep "${CMAKE_MATCH_1}")

  # Skip Windows system DLLs; bundle everything else (the MSYS2 prefix libs).
  string(TOLOWER "${_obk_dep}" _obk_dep_lower)
  if(_obk_dep_lower MATCHES "/windows/|/system32/|/syswow64/")
    continue()
  endif()

  # Translate the MSYS2 path (e.g. /clangarm64/bin/libc++.dll) into a Windows
  # path so the native CMake file(INSTALL) can read it.
  execute_process(
    COMMAND "${_obk_cygpath}" -w "${_obk_dep}"
    OUTPUT_VARIABLE _obk_win
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(EXISTS "${_obk_win}")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}" TYPE SHARED_LIBRARY FILES "${_obk_win}")
  endif()
endforeach()
