# - Manage files
#
# Included Modules:
#   - ManageMessage
#   - ManageVariable
#
# Defines following functions:
#   FIND_FILE_ERROR_HANDLING(<var>
#     [ERROR_MSG <errorMessage>]
#     [ERROR_VAR <errorVar>]
#     [VERBOSE_LEVEL <verboseLevel>]
#     [FIND_ARGS ...]
#     )
#     - Find a file, with proper error handling.
#       It is essentially a wrapper of FIND_FILE
#       * Parameters:
#         + var: The variable that stores the path of the found program.
#         + name: The filename of the command.
#         + verboseLevel: See ManageMessage for semantic of 
#           each verbose level.
#         + ERROR_MSG errorMessage: Error message to be append.
#         + ERROR_VAR errorVar: Variable that will be  set to 1 
#           when not found.
#         + FIND_ARGS: A list of arguments to be passed 
#           to FIND_FILE
#
#   FIND_PROGRAM_ERROR_HANDLING(<var>
#     [ERROR_MSG <errorMessage>]
#     [ERROR_VAR <errorVar>]
#     [VERBOSE_LEVEL <verboseLevel>]
#     [FIND_ARGS ...]
#     )
#     - Find an executable program, with proper error handling.
#       It is essentially a wrapper of FIND_PROGRAM
#       * Parameters:
#         + var: The variable that stores the path of the found program.
#         + name: The filename of the command.
#         + verboseLevel: See ManageMessage for semantic of 
#           each verbose level.
#         + ERROR_MSG errorMessage: Error message to be append.
#         + ERROR_VAR errorVar: Variable that will be  set to 1 
#           when not found.
#         + FIND_ARGS: A list of arguments to be passed 
#             to FIND_PROGRAM
#
#   GIT_GLOB_TO_CMAKE_REGEX(<var> <glob>)
#     - Convert git glob to cmake file regex
#       This macro covert git glob used in gitignore to
#       cmake file regex used in CPACK_SOURCE_IGNORE_FILES
#       * Parameters:
#         + var: Variable that hold the result.
#         + glob: Glob to be converted
#
#   MANAGE_CMAKE_FEDORA_CONF(<var>
#     [ERROR_MSG <errorMessage>]
#     [ERROR_VAR <errorVar>]
#     [VERBOSE_LEVEL <verboseLevel>]
#     )
#     - Locate cmake-fedora.conf
#       Return the location of cmake-fedora.conf.
#       It search following places:
#       ${CMAKE_SOURCE_DIR}, ${CMAKE_SOURCE_DIR}/cmake-fedora,
#       current dir, ./cmake-fedora and /etc.
#       * Parameters:
#         + var: The variable that returns the path of cmake-fedora.conf
#         + verboseLevel: See ManageMessage for semantic of 
#           each verbose level.
#         + ERROR_MSG errorMessage: Error message to be append.
#         + ERROR_VAR errorVar: This variable will be set to 1
#           when cmake-fedora.conf is not found.
#
#   MANAGE_FILE_COMMON_DIR(<var> <file1> ...>)
#     - Get the longest common path. 
#       Note that this function just do string comparison, it does not convert to real path.
#       It is recommended to use all absolute paths or all relative paths.
#       * Parameters:
#         + var: The variable that returns the file expiry status.
#           Valid status: ERROR, NOT_FOUND, EXPIRED, NOT_EXPIRED.
#         + file1 ...: files to compare.
#
#
#   MANAGE_FILE_CACHE(<var> <file> [EXPIRY_SECONDS <expirySecond>]
#     [CACHE_DIR <dir>] [ERROR_VAR <errorVar>] [RESULT_VAR resultVar]
#     COMMAND <cmd ...>
#     )
#     - Manage cached program output.
#       If cache is not existed, it runs the command and create the cache;
#       otherwise if cache is not expired, it returns the cached content;
#       otherwise if cache is expired, it run sthe command and update the 
#       cache.
#       Then this program returns cache content.
#       * Parameters:
#         + var: The variable the stores the content of the cache file.
#         + file: File to be processed. 
#         + EXPIRY_SECONDS expirySecond: (Optional) Seconds 
#           before the file expired.
#           If not specified, it will use the value LOCAL_CACHE_EXPIRY 
#           in cmake-fedora.conf, or 259200 (3 days).
#         + CACHE_DIR dir: (Optional) Directory of <file>.
#           If not specified, it will use the value 
#           LOCAL_CACHE_DIR in cmake-fedora.conf,
#           or $ENV{HOME}/.cache/cmake-fedora .
#         + ERROR_VAR errorVar: This variable will be set to 1
#           when cache cannot be created.
#         + RESULT_VAR resultVar: This variable returns the exit code
#           for <cmd>.
#         + COMMAND <cmd ...>: Command to produceoutput.
#
#   MANAGE_FILE_EXPIRY(<var> <file> <expirySecond>)
#     - Tell whether a file is expired.
#       A file is deemed as expired if (currenTime - mtime) is greater
#       than specified expiry time in seconds.
#       * Parameters:
#         + var: The variable that returns the file expiry status.
#           Valid status: ERROR, NOT_FOUND, EXPIRED, NOT_EXPIRED.
#         + file: File to be processed.
#         + expirySecond: Seconds before the file expired.
#
#
# Defines following macros:
#   MANAGE_FILE_INSTALL(<fileType>
#     [<files> | FILES <files>] [DEST_SUBDIR <subDir>] 
#     [RENAME <newName>] [ARGS <args>]
#     )
#     - (Deprecated) Manage file installation.
#       You can use cmake built-in INSTALL after cmake-fedora-2.0.0
#       * Parameters:
#         + fileType: Type of files. Valid values:
#           BIN, PRJ_DOC, DATA, PRJ_DATA, 
#           SYSCONF, SYSCONF_NO_REPLACE, 
#           LIB, LIBEXEC, TARGETS
#         + DEST_SUBDIR subDir: Subdir of Destination dir
#         + files: Files to be installed.
#         + RENAME newName: Destination filename.
#         + ARGS args: Arguments for INSTALL.
#

IF(DEFINED _MANAGE_FILE_CMAKE_)
    RETURN()
ENDIF(DEFINED _MANAGE_FILE_CMAKE_)
SET(_MANAGE_FILE_CMAKE_ "DEFINED")
SET(FILE_INSTALL_LIST_TYPES 
    "BIN" "PRJ_DOC" "DATA" "PRJ_DATA" "SYSCONF" "SYSCONF_NO_REPLACE"
    "LIB" "LIBEXEC"
    )
INCLUDE(ManageMessage RESULT_VARIABLE MANAGE_MODULE_PATH)
GET_FILENAME_COMPONENT(CMAKE_FEDORA_MODULE_DIR "${MANAGE_MODULE_PATH}" PATH)
INCLUDE(ManageVariable)
INCLUDE(ManageString)

## Common path between two
FUNCTION(MANAGE_FILE_COMMON_DIR_2 var file1 file2 separator)
    SET(result "")
    IF("${file1}" STREQUAL "${file2}")
	SET(result "${file1}")
    ELSE()
	STRING_SPLIT(dirA1 "${separator}" "${file1}")
	LIST(LENGTH dirA1 dirA1Len)
	STRING_SPLIT(dirA2 "${separator}" "${file2}")
	LIST(LENGTH dirA2 dirA2Len)
	SET(i 0)
	WHILE(i LESS dirA1Len)
	    IF(NOT i LESS dirA2Len)
		BREAK()
	    ENDIF()
	    LIST(GET dirA1 ${i} token1)
	    LIST(GET dirA2 ${i} token2)
	    IF(NOT "${token1}" STREQUAL "${token2}")
		BREAK()
	    ENDIF()
	    STRING_APPEND(result "${token1}" "${separator}")
	    MATH(EXPR i ${i}+1)
	ENDWHILE()
    ENDIF()
    SET(${var} "${result}" PARENT_SCOPE)
ENDFUNCTION(MANAGE_FILE_COMMON_DIR_2)

FUNCTION(MANAGE_FILE_COMMON_DIR var file1)
    SET(separator "/")
    IF(EXISTS "${file1}")
	IF(IS_DIRECTORY "${file1}")
	    SET(commonDir "${file1}")
	ELSE()
	    GET_FILENAME_COMPONENT(commonDir "${file1}" PATH)
	ENDIF()
    ELSE()
	SET(commonDir "${file1}")
    ENDIF()

    FOREACH(f ${ARGN})
	MANAGE_FILE_COMMON_DIR_2(commonDir "${commonDir}" "${f}" "${separator}")
	IF("${commonDir}" STREQUAL "")
	    BREAK()
	ENDIF()
    ENDFOREACH(f)
    SET(${var} "${commonDir}" PARENT_SCOPE)
ENDFUNCTION(MANAGE_FILE_COMMON_DIR)

MACRO(_MANAGE_FILE_SET_FILE_INSTALL_LIST fileType)
    SET(FILE_INSTALL_${fileType}_LIST "${FILE_INSTALL_${fileType}_LIST}"
	CACHE INTERNAL "List of files install as ${fileType}" FORCE
	)
ENDMACRO(_MANAGE_FILE_SET_FILE_INSTALL_LIST fileType)

FOREACH(_fLT ${FILE_INSTALL_LIST_TYPES})
    SET(FILE_INSTALL_${_fLT}_LIST "")
    _MANAGE_FILE_SET_FILE_INSTALL_LIST(${_fLT})
ENDFOREACH(_fLT ${FILE_INSTALL_LIST_TYPES})

MACRO(_MANAGE_FILE_INSTALL_FILE_OR_DIR fileType)
    IF(_opt_RENAME)
	SET(_install_options "RENAME" "${_opt_RENAME}")
    ELSE(_opt_RENAME)
	SET(_install_options "")
    ENDIF (_opt_RENAME)
    FOREACH(_f ${_fileList})
	GET_FILENAME_COMPONENT(_a "${_f}" ABSOLUTE)
	SET(_absolute "")
	STRING(REGEX MATCH "^/" _absolute "${_f}")
	IF(IS_DIRECTORY "${_a}") 
	    SET(_install_type "DIRECTORY")
	ELSE(IS_DIRECTORY "${_a}")
	    IF("${fileType}" STREQUAL "BIN")
		SET(_install_type "PROGRAMS")
	    ELSE("${fileType}" STREQUAL "BIN")
		SET(_install_type "FILES")
	    ENDIF("${fileType}" STREQUAL "BIN")
	ENDIF(IS_DIRECTORY "${_a}")
	INSTALL(${_install_type} ${_f} DESTINATION "${_destDir}"
	    ${_install_options} ${ARGN})
	IF(_opt_RENAME)
	    SET(_n "${_opt_RENAME}")
	ELSEIF(_absolute)
	    GET_FILENAME_COMPONENT(_n "${_f}" NAME)
	ELSE(_opt_RENAME)
	    SET(_n "${_f}")
	ENDIF(_opt_RENAME)

	IF(_opt_DEST_SUBDIR)
	    LIST(APPEND FILE_INSTALL_${fileType}_LIST
		"${_opt_DEST_SUBDIR}/${_n}")
	ELSE(_opt_DEST_SUBDIR)
	    LIST(APPEND FILE_INSTALL_${fileType}_LIST
		"${_n}")
	ENDIF(_opt_DEST_SUBDIR)
    ENDFOREACH(_f ${_fileList})
    _MANAGE_FILE_SET_FILE_INSTALL_LIST("${fileType}")

ENDMACRO(_MANAGE_FILE_INSTALL_FILE_OR_DIR fileType)

MACRO(_MANAGE_FILE_INSTALL_TARGET)
    SET(_installValidOptions "RUNTIME" "LIBEXEC" "LIBRARY" "ARCHIVE")
    VARIABLE_PARSE_ARGN(_oT _installValidOptions ${ARGN})
    SET(_installOptions "")
    FOREACH(_f ${_fileList})
	GET_TARGET_PROPERTY(_tP "${_f}" TYPE)
	IF(_tP STREQUAL "EXECUTABLE")
	    LIST(APPEND _installOptions RUNTIME)
	    IF(_oT_RUNTIME)
		LIST(APPEND FILE_INSTALL_BIN_LIST ${_f})
		_MANAGE_FILE_SET_FILE_INSTALL_LIST("BIN")
		LIST(APPEND _installOptions "${_oT_RUNTIME}")
	    ELSEIF(_oT_LIBEXEC)
		LIST(APPEND FILE_INSTALL_LIBEXEC_LIST ${_f})
		_MANAGE_FILE_SET_FILE_INSTALL_LIST("LIBEXEC")
		LIST(APPEND _installOptions "${_oT_LIBEXEC}")
	    ELSE(_oT_RUNTIME)
		M_MSG(${M_ERROR} 
		    "MANAGE_FILE_INSTALL_TARGETS: Type ${_tP} is not yet implemented.")
	    ENDIF(_oT_RUNTIME)
	ELSEIF(_tP STREQUAL "SHARED_LIBRARY")
	    LIST(APPEND FILE_INSTALL_LIB_LIST ${_f})
	    _MANAGE_FILE_SET_FILE_INSTALL_LIST("LIB")
	    LIST(APPEND _installOptions "LIBRARY" "${_oT_LIBRARY}")
	ELSEIF(_tP STREQUAL "STATIC_LIBRARY")
	    M_MSG(${M_OFF} 
		"MANAGE_FILE_INSTALL_TARGETS: Fedora does not recommend type ${_tP}, excluded from rpm")
	    LIST(APPEND _installOptions "ARCHIVE" "${_oT_ARCHIVE}")
	ELSE(_tP STREQUAL "EXECUTABLE")
	    M_MSG(${M_ERROR} 
		"MANAGE_FILE_INSTALL_TARGETS: Type ${_tP} is not yet implemented.")
	ENDIF(_tP STREQUAL "EXECUTABLE")
    ENDFOREACH(_f ${_fileList})
    INSTALL(TARGETS ${_fileList} ${_installOptions})
ENDMACRO(_MANAGE_FILE_INSTALL_TARGET)

MACRO(MANAGE_FILE_INSTALL fileType)
    SET(_validOptions "DEST_SUBDIR" "FILES" "ARGS" "RENAME")
    VARIABLE_PARSE_ARGN(_opt _validOptions ${ARGN})
    SET(_fileList "")
    LIST(APPEND _fileList ${_opt} ${_opt_FILES})

    IF("${fileType}" STREQUAL "SYSCONF_NO_REPLACE")
	SET(_destDir "${SYSCONF_DIR}/${_opt_DEST_SUBDIR}")
	_MANAGE_FILE_INSTALL_FILE_OR_DIR("${fileType}")
    ELSEIF("${fileType}" STREQUAL "TARGETS")
	_MANAGE_FILE_INSTALL_TARGET(${_opt_ARGS})
    ELSE("${fileType}" STREQUAL "SYSCONF_NO_REPLACE")
	SET(_destDir "${${fileType}_DIR}/${_opt_DEST_SUBDIR}")
	_MANAGE_FILE_INSTALL_FILE_OR_DIR("${fileType}")
    ENDIF("${fileType}" STREQUAL "SYSCONF_NO_REPLACE")
ENDMACRO(MANAGE_FILE_INSTALL fileType)

FUNCTION(FIND_ERROR_HANDLING type vari)
    SET(_verboseLevel ${M_ERROR})
    SET(_errorMsg "")
    SET(_errorVar "")
    SET(_findFileArgList "")
    SET(_state "")
    FOREACH(_arg ${ARGN})
	IF(_state STREQUAL "ERROR_MSG")
	    SET(_errorMsg "${_arg}")
	    SET(_state "")
	ELSEIF(_state STREQUAL "ERROR_VAR")
	    SET(_errorVar "${_arg}")
	    SET(_state "")
	ELSEIF(_state STREQUAL "VERBOSE_LEVEL")
	    SET(_verboseLevel "${_arg}")
	    SET(_state "")
	ELSEIF(_state STREQUAL "FIND_ARGS")
	    LIST(APPEND _findFileArgList "${_arg}")
	ELSE(_state STREQUAL "ERROR_MSG")
	    IF(_arg STREQUAL "ERROR_MSG")
		SET(_state "${_arg}")
	    ELSEIF(_arg STREQUAL "ERROR_VAR")
		SET(_state "${_arg}")
	    ELSEIF(_arg STREQUAL "VERBOSE_LEVEL")
		SET(_state "${_arg}")
	    ELSE(_arg STREQUAL "ERROR_MSG")
		SET(_state "FIND_ARGS")
		IF(NOT _arg STREQUAL "FIND_ARGS")
		    LIST(APPEND _findFileArgList "${_arg}")
		ENDIF(NOT _arg STREQUAL "FIND_ARGS")
	    ENDIF(_arg STREQUAL "ERROR_MSG")
	ENDIF(_state STREQUAL "ERROR_MSG")
    ENDFOREACH(_arg ${ARGN})

    ## FIND_PROGRAM and FIND_FILE caches the result
    ## And won't find again until the cache is cleaned
    IF("${type}" STREQUAL "PROGRAM")
	SET(_type "Program")
	FIND_PROGRAM(${vari} ${_findFileArgList})
    ELSE("${type}" STREQUAL "PROGRAM")
	SET(_type "File")
	FIND_FILE(${vari} ${_findFileArgList})
    ENDIF("${type}" STREQUAL "PROGRAM")

    IF("${${vari}}" STREQUAL "${vari}-NOTFOUND")
	IF(NOT _errorMsg)
	    SET(_str "")
	    FOREACH(_s ${_findFileArgList})
		SET(_str "${_str} ${_s}")
	    ENDFOREACH(_s ${_findFileArgList})

	    SET(_errorMsg "${_type} cannot be found with following arguments: ${_str}")
	ENDIF(NOT _errorMsg)

	M_MSG(${_verboseLevel} "${_errorMsg}")
	IF (NOT _errorVar STREQUAL "")
	    SET(${_errorVar} 1 PARENT_SCOPE)
	ENDIF(NOT _errorVar STREQUAL "")
    ENDIF("${${vari}}" STREQUAL "${vari}-NOTFOUND")
ENDFUNCTION(FIND_ERROR_HANDLING type vari)

FUNCTION(FIND_FILE_ERROR_HANDLING var)
    FIND_ERROR_HANDLING(FILE ${var} ${ARGN})
ENDFUNCTION(FIND_FILE_ERROR_HANDLING var)

FUNCTION(FIND_PROGRAM_ERROR_HANDLING var)
    FIND_ERROR_HANDLING(PROGRAM ${var} ${ARGN})
ENDFUNCTION(FIND_PROGRAM_ERROR_HANDLING var)

FUNCTION(MANAGE_CMAKE_FEDORA_CONF var)
    FIND_FILE_ERROR_HANDLING(${var} ${ARGN}
	FIND_ARGS cmake-fedora.conf 
	PATHS ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/cmake-fedora
	. cmake-fedora /etc 
	${CMAKE_FEDORA_MODULE_DIR}/..
	${CMAKE_FEDORA_MODULE_DIR}/../cmake-fedora
	${CMAKE_SOURCE_DIR}/../../..
	${CMAKE_SOURCE_DIR}/../../../cmake-fedora
	${CMAKE_SOURCE_DIR}/../..
	${CMAKE_SOURCE_DIR}/../../cmake-fedora
	)
    SET(${var} "${${var}}" PARENT_SCOPE)
ENDFUNCTION(MANAGE_CMAKE_FEDORA_CONF var)

FUNCTION(MANAGE_FILE_CACHE var file)
    SET(_validOptions "CACHE_DIR" 
	"EXPIRY_SECONDS" "ERROR_VAR" "RESULT_VAR" "COMMAND")
    VARIABLE_PARSE_ARGN(_o _validOptions ${ARGN})
    IF(NOT DEFINED _o_ERROR_VAR)
	SET(_o_ERROR_VAR "${var}_ERROR")
	SET(${var}_ERROR 0)
    ENDIF(NOT DEFINED _o_ERROR_VAR)
    SET(_commandOptList "")
    IF(DEFINED _o_RESULT_VAR)
	SET(_commandOptList RESULT_VARIABLE ${_o_RESULT_VAR})
    ENDIF(DEFINED _o_RESULT_VAR)

    CMAKE_FEDORA_CONF_GET_ALL_VARIABLES()
    SET(_toRun TRUE)
    IF(NOT DEFINED LOCAL_CACHE)
	SET(LOCAL_CACHE 1)
    ENDIF(NOT DEFINED LOCAL_CACHE)
    IF(LOCAL_CACHE)
	IF(NOT _o_CACHE_DIR)
	    IF(LOCAL_CACHE_DIR)
		SET(_o_CACHE_DIR ${LOCAL_CACHE_DIR})
	    ELSE(LOCAL_CACHE_DIR)
		SET(_o_CACHE_DIR "${HOME}/.cache/cmake-fedora")
	    ENDIF(LOCAL_CACHE_DIR)
	ENDIF(NOT _o_CACHE_DIR)
	IF(NOT _o_EXPIRY_SECONDS)
	    IF(LOCAL_CACHE_EXPIRY)
		SET(_o_EXPIRY_SECONDS ${LOCAL_CACHE_EXPIRY})
	    ELSE(LOCAL_CACHE_EXPIRY)
		SET(_o_EXPIRY_SECONDS 259200) # 3 days
	    ENDIF(LOCAL_CACHE_EXPIRY)
	ENDIF(NOT  _o_EXPIRY_SECONDS)

	IF(NOT EXISTS ${_o_CACHE_DIR})
	    EXECUTE_PROCESS(COMMAND 
		${CMAKE_COMMAND} -E make_directory "${_o_CACHE_DIR}"
		RESULT_VARIABLE ${_o_ERROR_VAR}
		OUTPUT_QUIET
		ERROR_QUIET
		)
	ENDIF(NOT EXISTS ${_o_CACHE_DIR})
	IF(NOT ${${_o_ERROR_VAR}} EQUAL 0)
	    ## Error when creating cache dir
	    RETURN()
	ENDIF(NOT ${${_o_ERROR_VAR}} EQUAL 0)

	SET(_cacheFile "${_o_CACHE_DIR}/${file}")
	MANAGE_FILE_EXPIRY(_isExpired ${_cacheFile} ${_o_EXPIRY_SECONDS})
	IF(_isExpired STREQUAL "NOT_EXIST")
	    SET(_toRun TRUE)
	ELSEIF(_isExpired STREQUAL "NOT_EXPIRED")
	    SET(_toRun FALSE)
	ELSEIF(_isExpired STREQUAL "EXPIRED")
	    SET(_toRun TRUE)
	ELSE(_isExpired STREQUAL "NOT_EXIST")
	    M_MSG(${M_ERROR} "Failed on checking file expirary")
	ENDIF(_isExpired STREQUAL "NOT_EXIST")
    ELSE(LOCAL_CACHE)
	SET(_cacheFile "/tmp/cmake_fedora_cache_${cache_file}")
    ENDIF(LOCAL_CACHE)
    
    IF(_toRun)
	EXECUTE_PROCESS(COMMAND ${_o_COMMAND}
	    ${_commandOptList}
	    OUTPUT_FILE ${_cacheFile}
	    OUTPUT_STRIP_TRAILING_WHITESPACE
	    )
    ENDIF(_toRun)
    FILE(READ ${_cacheFile} _value)
    STRING(STRIP "${_value}" _value)
    SET(${var} "${_value}" PARENT_SCOPE)
ENDFUNCTION(MANAGE_FILE_CACHE)

FUNCTION(MANAGE_FILE_EXPIRY var file expirySecond)
    IF(EXISTS "${file}")
	EXECUTE_PROCESS(COMMAND stat --format "%Y" "${file}"
	    OUTPUT_VARIABLE _fileTime
	    OUTPUT_STRIP_TRAILING_WHITESPACE
	    )
	EXECUTE_PROCESS(COMMAND date "+%s"
	    OUTPUT_VARIABLE _currentTime
	    OUTPUT_STRIP_TRAILING_WHITESPACE
	    )
	MATH(EXPR _expireAt "${_fileTime}+${expirySecond}")
	IF(_currentTime LESS _expireAt)
	    ## Not Expired
	    SET(${var} "NOT_EXPIRED" PARENT_SCOPE)
	ELSE(_currentTime LESS _expireAt)
	    SET(${var} "EXPIRED" PARENT_SCOPE)
	ENDIF(_currentTime LESS _expireAt)
    ELSE(EXISTS "${file}")
	SET(${var} "NOT_EXIST" PARENT_SCOPE)
    ENDIF(EXISTS "${file}")
ENDFUNCTION(MANAGE_FILE_EXPIRY var file expirySecond)

FUNCTION(GIT_GLOB_TO_CMAKE_REGEX var glob)
    SET(_s "${glob}")
    STRING(REGEX REPLACE "!" "!e" _s "${_s}")
    STRING(REGEX REPLACE "[*]{2}" "!d" _s "${_s}")
    STRING(REGEX REPLACE "[*]" "!s" _s "${_s}")
    STRING(REGEX REPLACE "[?]" "!q" _s "${_s}")
    STRING(REGEX REPLACE "[.]" "\\\\\\\\." _s "${_s}")
    STRING(REGEX REPLACE "!d" ".*" _s "${_s}")
    STRING(REGEX REPLACE "!s" "[^/]*" _s "${_s}")
    STRING(REGEX REPLACE "!q" "[^/]" _s "${_s}")
    STRING(REGEX REPLACE "!e" "!" _s "${_s}")
    STRING(LENGTH "${_s}" _len)
    MATH(EXPR _l ${_len}-1)
    STRING(SUBSTRING "${_s}" ${_l} 1 _t)
    IF( _t STREQUAL "/")
	SET(_s "/${_s}")
    ELSE( _t STREQUAL "/")
	SET(_s "${_s}\$")
    ENDIF( _t STREQUAL "/")
    SET(${var} "${_s}" PARENT_SCOPE)
ENDFUNCTION(GIT_GLOB_TO_CMAKE_REGEX)

