# - Dependency Management Module
# This module handle dependencies by using pkg-config and/or
# search the executable.
# 
# Included Modules:
#  - ManageFile
#  - ManageString
#  - ManageVariable
#
# Variable to be read:
#  + MANAGE_DEPENDENCY_PACKAGE_EXISTS_CMD: Command and options that check
#      the existence of a package
#    Default: rpm -q
#
#  + MANAGE_DEPENDENCY_PACKAGE_INSTALL_CMD: Command and options that install
#      a package
#    Default: yum -y install
#
# Defines following functions:
#   MANAGE_DEPENDENCY(<listVar> <var> [VER <ver> [EXACT]] [REQUIRED] 
#     [PROGRAM_NAMES <name1> ...] [PROGRAM_SEARCH_PATHS <path1> ...] 
#     [PKG_CONFIG <pkgConfigName1> ...]
#     [PACKAGE_NAME <packageName> | DEVEL]
#     )
#     - Add a new dependency to a list. 
#       The dependency will also be searched.
#       If found, ${var}_FOUND is set as 1.
#       If not found:
#         + If REQUIRED is specified: a M_ERROR message will be printed. #	     + If REQUIRED is not specified: a M_OFF message will be printed.
#       See "Variables to cache" for the variable overridden and output.
#       * Parameters:
#         + listVar: List variable store a kind of dependencies.
#           Recognized lists:
#           - BUILD_REQUIRES: Dependencies in build stage
#           - REQUIRES:       Dependencies for runtime
#           - REQUIRES_PRE:   Dependencies before the package install
#           - REQUIRES_PREUN: Dependencies before the package uninstall
#           - REQUIRES_POST:  Dependencies after the package install
#           - REQUIRES_POSTUN:Dependencies after the package uninstall
#         + var: Main variable. Uppercase variable name is recommended,
#           (e.g. GETTEXT)
#         + VER ver [EXACT]: Minimum version.
#           Specify the exact version by providing "EXACT".
#         + REQUIRED: The dependency is required at cmake build time.
#         + PROGRAM_NAMES name1 ...: Executable to be found.
#             name2 and others are aliases to name1.
#             If found, ${var}_EXECUTABLE is defined as the full path 
#             to the executable; if not found; the whole dependency is
#             deemed as not found.
#         + PROGRAM_SEARCH_PATHS path1 ...: Additional program search path.
#             It will act as PATHS arguments for FIND_PROGRAM.
#         + PKG_CONFIG pkgConfigName1 ...: List of the pkg-config file
#             exclude the directory and .pc. e.g. "gtk+-2.0"
#         + PACKAGE_NAME packageName: The actual package name in repository. 
#             If not specified, use the lowercase of ${var}.
#             For example, use following to specify libchewing as dependency 
#             under the name CHEWING:
#           MANAGE_DEPENDENCY(REQUIRES CHEWING PACKAGE_NAME "libchewing")
#         + DEVEL: search lowercase of ${var}-devel.
#             A shortcut to PACKAGE_NAME ${var}-devel.
#       * Variables to cache:
#         + ${listVar}_${listVar}_${var}_PACKAGE_NAME: The actual package name in repository.
#             Override this if your system is different from Fedora.
#         + ${listVar}_${var}_PKG_CONFIG: List of pkg-config files.
#             Override this if your system is different from Fedora.
#         + ${var}_<print_variable>: The pkg-config variables.
#             For example, datadir=/usr/share/chewing will be
#           ${var}_DATADIR, whose value is "/usr/share/chewing"
#
IF(DEFINED _MANAGE_DEPENDENCY_CMAKE_)
    RETURN()
ENDIF(DEFINED _MANAGE_DEPENDENCY_CMAKE_)
SET(_MANAGE_DEPENDENCY_CMAKE_ "DEFINED")
INCLUDE(ManageFile)
INCLUDE(ManageVariable)
INCLUDE(ManageString)
SET(MANAGE_DEPENDENCY_PACKAGE_EXISTS_CMD rpm -q 
    CACHE STRING "Package exist command"
    )

SET(MANAGE_DEPENDENCY_PACKAGE_INSTALL_CMD yum -y install 
    CACHE STRING "Package exist command"
    )

## This need to be here, otherwise the variable won't be available
## the 2nd time called.
FIND_PACKAGE(PkgConfig)

## This is declared as function, because 
## macro does not play nice if listVar is required in different
## source dir.
FUNCTION(MANAGE_DEPENDENCY listVar var)
    SET(_validOptions "VER" "EXACT" "REQUIRED" 
	"PROGRAM_NAMES" "PROGRAM_SEARCH_PATHS" "PKG_CONFIG" "PACKAGE_NAME" "DEVEL")
    VARIABLE_PARSE_ARGN(_opt _validOptions ${ARGN})
    SET(_dirty 0)

    IF("${${listVar}_${var}_PACKAGE_NAME}" STREQUAL "")
	IF(_opt_PACKAGE_NAME)
	    IF(DEFINED _opt_DEVEL)
		M_MSG(${M_ERROR} "PACKAGE_NAME cannot use with DEVEL")
	    ENDIF()
	    SET(${listVar}_${var}_PACKAGE_NAME "${_opt_PACKAGE_NAME}")
	ELSE(_opt_PACKAGE_NAME)
	    STRING(TOLOWER "${var}" ${listVar}_${var}_PACKAGE_NAME)
	    IF(DEFINED _opt_DEVEL)
		STRING_APPEND(${listVar}_${var}_PACKAGE_NAME "-devel")
	    ENDIF()
	ENDIF(_opt_PACKAGE_NAME)
	SET(${listVar}_${var}_PACKAGE_NAME "${${listVar}_${var}_PACKAGE_NAME}" 
	    CACHE STRING "${listVar}_${var}_PACKAGE_NAME")
    ENDIF()
    SET(pkgName "${${listVar}_${var}_PACKAGE_NAME}")

    IF(DEFINED _opt_REQUIRED)
	SET(_verbose "${M_ERROR}")
	SET(_required "REQUIRED")
	SET(_progNotFoundMsg 
	    "Program names ${_opt_PROGRAM_NAMES} not found, install ${pkgName}")
    ELSE(DEFINED _opt_REQUIRED)
	SET(_verbose "${M_OFF}")
	SET(_required "")
	SET(_progNotFoundMsg 
	    "Program names ${_opt_PROGRAM_NAMES} not found, ${var} support disabled")
    ENDIF(DEFINED _opt_REQUIRED)

    IF(_opt_VER)
	IF(DEFINED _opt_EXACT)
	    SET(_rel "=")
	    SET(_exact "EXACT")
	ELSE(DEFINED _opt_EXACT)
	    SET(_rel ">=")
	    SET(_exact "")
	ENDIF(DEFINED _opt_EXACT)
    ENDIF(_opt_VER)

    IF(_opt_PROGRAM_NAMES)
	M_MSG(${M_INFO2} "ManageDependency: Finding program names ${_opt_PROGRAM_NAMES}")
	SET(_findArgs FIND_ARGS NAMES "${_opt_PROGRAM_NAMES}")
	IF(_opt_PROGRAM_SEARCH_PATHS)
	    LIST(APPEND _findArgs PATHS ${_opt_PROGRAM_SEARCH_PATHS})
	ENDIF()
	FIND_PROGRAM_ERROR_HANDLING(${var}_EXECUTABLE
	    ERROR_VAR _dirty
	    ERROR_MSG "${_progNotFoundMsg}"
	    VERBOSE_LEVEL "${_verbose}"
	    ${_findArgs}
	    )
	MARK_AS_ADVANCED(${var}_EXECUTABLE)
    ENDIF(_opt_PROGRAM_NAMES)

    IF("${_opt_VER}" STREQUAL "")
	SET(_newDep  "${pkgName}")
    ELSE("${_opt_VER}" STREQUAL "")
	SET(_newDep  "${pkgName} ${_rel} ${_opt_VER}")
    ENDIF("${_opt_VER}" STREQUAL "")

    ## Check package exist
    SET(pkgExistsCmdMissing 0)
    LIST(GET MANAGE_DEPENDENCY_PACKAGE_EXISTS_CMD 0 pkgExistsCmd)
    FIND_PROGRAM_ERROR_HANDLING(PKG_EXISTS_CMD
	ERROR_VAR pkgExistsCmdMissing
	ERROR_MSG "ManageDependency: Program ${pkgExistsCmd} not found, dependency check disabled."
	VERBOSE_LEVEL ${M_OFF}
	FIND_ARGS ${pkgExistsCmd}
	)

    IF(NOT pkgExistsCmdMissing)
	EXECUTE_PROCESS(COMMAND ${MANAGE_DEPENDENCY_PACKAGE_EXISTS_CMD} ${pkgName}
	    RESULT_VARIABLE pkgMissing
	    OUTPUT_QUIET
	    ERROR_QUIET
	    )
	IF(pkgMissing)
	    ## Dependency not found
	    M_MSG(${_verbose} "Package ${pkgName} is not installed")
	    SET(_dirty 1)
	ENDIF(pkgMissing)
    ENDIF()

    ## PKG_CONFIG
    IF("${${listVar}_${var}_PKG_CONFIG}" STREQUAL "")
	IF(_opt_PKG_CONFIG)
	    SET(${listVar}_${var}_PKG_CONFIG "${_opt_PKG_CONFIG}" 
		CACHE STRING "${listVar}_${var}_PKG_CONFIG")
	ENDIF(_opt_PKG_CONFIG)
    ENDIF()
    SET(pkgConf "${${listVar}_${var}_PKG_CONFIG}")

    IF(pkgConf)
	IF(PKG_CONFIG_EXECUTABLE)
	    ## Add pkgconfig itself as dependency
	    SET(PKG_CONFIG_PACKAGE_NAME "pkgconfig" 
		CACHE STRING "PKG_CONFIG_PACKAGE_NAME")
	    LIST(FIND ${listVar} "${PKG_CONFIG_PACKAGE_NAME}" _index)
	    IF(_index EQUAL -1)
		LIST(APPEND ${listVar} "${PKG_CONFIG_PACKAGE_NAME}")
	    ENDIF(_index EQUAL -1)
	ELSE(PKG_CONFIG_EXECUTABLE)
	    M_MSG(${M_ERROR} "pkgconfig is required with ${var}")
	ENDIF(PKG_CONFIG_EXECUTABLE)
	SET(pCList "")
	FOREACH(pC ${pkgConf})
	    LIST(APPEND pCList "${pC}${_rel}${_opt_VER}")
	    ## Get all variables
	    EXECUTE_PROCESS(COMMAND ${PKG_CONFIG_EXECUTABLE}
		--print-variables "${pC}"
		OUTPUT_VARIABLE _variables
		OUTPUT_STRIP_TRAILING_WHITESPACE
		RESULT_VARIABLE pkgconfigFailed
		)
	    IF(NOT pkgconfigFailed)
		STRING_SPLIT(${var}_VARIABLES "\n" "${_variables}")
		FOREACH(_v ${${var}_VARIABLES})
		    STRING(TOUPPER "${_v}" _u)
		    EXECUTE_PROCESS(COMMAND ${PKG_CONFIG_EXECUTABLE}
			--variable "${_v}" "${pC}"
			OUTPUT_VARIABLE ${var}_${pC}_${_u}
			OUTPUT_STRIP_TRAILING_WHITESPACE
			)
		    SET(${var}_${pC}_${_u} "${${var}_${pC}_${_u}}" 
			CACHE INTERNAL "pkgconfig ${${var}_${pC}_${_u}}")

		    MARK_AS_ADVANCED(${${var}_${pC}_${_u}})
		    M_MSG(${M_INFO1} "${var}_${pC}_${_u}=${${var}_${pC}_${_u}}")
		ENDFOREACH(_v)

	    ENDIF(NOT pkgconfigFailed)
	ENDFOREACH(pC)
	PKG_CHECK_MODULES(${var} ${_required}
	    ${pCList})
    ENDIF(pkgConf)

    ## Insert when it's not duplicated
    IF(NOT _dirty)
	SET(${var}_FOUND "1" CACHE INTERNAL "Found ${var}")
    ENDIF(NOT _dirty)
    LIST(FIND ${listVar} "${_newDep}" _index)
    IF(_index EQUAL -1)
	LIST(APPEND ${listVar} "${_newDep}")
	SET(${listVar} "${${listVar}}" CACHE INTERNAL "${listVar} package list")
    ENDIF(_index EQUAL -1)
ENDFUNCTION(MANAGE_DEPENDENCY)

