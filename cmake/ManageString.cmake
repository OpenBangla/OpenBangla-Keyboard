# - Manage String by utility functions
#
# Defines the following functions:
#   STRING_APPEND(<var> <str> [<separator>])
#     - Append a string to a variable.
#       * Parameters:
#         + var: A variable that stores the result.
#         + str: A string to be appended to end of line.
#         + separator: Separator to separate between strings.
#
#   STRING_ESCAPE_BACKSLASH(<var> <str>)
#     - Escape the backslash (\).
#       * Parameters:
#         + var: A variable that stores the result.
#         + str: A string.
#
#   STRING_ESCAPE_DOLLAR(<var> <str>)
#     - Escape the dollar sign ($).
#       * Parameters:
#         + var: A variable that stores the result.
#         + str: A string.
#
#   STRING_ESCAPE_QUOTE(<var> <str>)
#     - Escape the double quote (").
#       * Parameters:
#         + var: A variable that stores the result.
#         + str: A string.
#
#   STRING_ESCAPE_SEMICOLON(<var> <str>)
#     - Escape the semicolon (;).
#       * Parameters:
#         + var: A variable that stores the result.
#         + str: A string.
#
#   STRING_PADDING(<var> <str> <length> [<padStr>])
#     - Padding the string to specified length.
#       * Parameters:
#         + var: A variable that stores the result.
#         + str: A string.
#         + length: Required length.
#         + padStr: String that used in padding. Default: " "
#
#   STRING_PREPEND(<var> <str> [<separator>])
#     - Prepend a string to a variable.
#       * Parameters:
#         + var: A variable that stores the result.
#         + str: A string to be appended to end of line.
#         + separator: Separator to separate between strings.
#
#   STRING_SPLIT(<var> <delimiter> <str> 
#       [NOESCAPE_SEMICOLON] [ESCAPE_VARIABLE] [ALLOW_EMPTY]
#     )
#     - Split a string into a list using a delimiter, 
#       which can be in 1 or more characters long.
#       * Parameters:
#         + var: A variable that stores the result.
#         + delimiter: To separate a string.
#         + str: A string.
#         + NOESCAPE_SEMICOLON: (Optional) Do not escape semicolons.
#         + ESCAPE_VARIABLE: (Optional) Escape variables.
#         + ALLOW_EMPTY: (Optional) Allow empty element exist in the array.
#
#   STRING_TRIM(<var> <str> [NOUNQUOTE])
#     - Trim a string by removing the leading and trailing spaces,
#       just like STRING(STRIP ...) in CMake 2.6 and later.
#       This macro was developed as CMake 2.4 does not support 
#        STRING(STRIP ..)
#       This macro also remove quote and double quote marks around 
#       the string, unless NOUNQUOTE is defined.
#       * Parameters:
#         + var: A variable that stores the result.
#         + str: A string.
#         + NOUNQUOTE: (Optional) do not remove the double quote mark
#           around the string.
#
#   STRING_UNQUOTE(<var> <str>)
#     - Remove double quote marks and quote marks around a string.
#       If the string is not quoted, then content of str is copied to var
#       * Parameters:
#         + var: A variable that stores the result.
#         + str: A string.
#
# Defines the following macros:
#   STRING_JOIN(<var> <delimiter> <strList> [<str> ...])
#     - Concatenate strings, with delimiter inserted between strings.
#       * Parameters:
#         + var: A variable that stores the result.
#         + strList: A list of strings.
#         + str: (Optional) more string to be join.
#

IF(DEFINED _MANAGE_STRING_CMAKE_)
    RETURN()
ENDIF(DEFINED _MANAGE_STRING_CMAKE_)
SET(_MANAGE_STRING_CMAKE_ "DEFINED")

FUNCTION(STRING_APPEND var str)
    IF(${ARGC} GREATER 2)
	SET(_sep "${ARGV2}")
    ELSE(${ARGC} GREATER 2)
	SET(_sep "")
    ENDIF(${ARGC} GREATER 2)
    IF (POLICY CMP0054)
	CMAKE_POLICY(PUSH)
	CMAKE_POLICY(SET CMP0054 "NEW")
    ENDIF()
    IF("${${var}}" STREQUAL "")
	SET(${var} "${str}" PARENT_SCOPE)
    ELSE("${${var}}" STREQUAL "")
	SET(${var} "${${var}}${_sep}${str}" PARENT_SCOPE)
    ENDIF("${${var}}" STREQUAL "")
    IF (POLICY CMP0054)
	CMAKE_POLICY(POP)
    ENDIF()
ENDFUNCTION(STRING_APPEND var str)

FUNCTION(STRING_ESCAPE_BACKSLASH var str)
    IF(str STREQUAL "")
	SET(${var} "" PARENT_SCOPE)
    ELSE(str STREQUAL "")
	STRING(REPLACE "\\" "\\\\" output "${str}")
	SET(${var} "${output}" PARENT_SCOPE)
    ENDIF(str STREQUAL "")
ENDFUNCTION(STRING_ESCAPE_BACKSLASH)

FUNCTION(STRING_ESCAPE_DOLLAR var str)
    IF(str STREQUAL "")
	SET(${var} "" PARENT_SCOPE)
    ELSE(str STREQUAL "")
	STRING(REPLACE "\$" "\\\$" output "${str}")
	SET(${var} "${output}" PARENT_SCOPE)
    ENDIF(str STREQUAL "")
ENDFUNCTION(STRING_ESCAPE_DOLLAR)

FUNCTION(STRING_ESCAPE_QUOTE var str)
    IF(str STREQUAL "")
	SET(${var} "" PARENT_SCOPE)
    ELSE(str STREQUAL "")
	STRING(REPLACE "\"" "\\\"" output "${str}")
	SET(${var} "${output}" PARENT_SCOPE)
    ENDIF(str STREQUAL "")
ENDFUNCTION(STRING_ESCAPE_QUOTE)

FUNCTION(STRING_ESCAPE_SEMICOLON var str)
    IF(str STREQUAL "")
	SET(${var} "" PARENT_SCOPE)
    ELSE(str STREQUAL "")
	STRING(REPLACE ";" "\\;" output "${str}")
	SET(${var} "${output}" PARENT_SCOPE)
    ENDIF(str STREQUAL "")
ENDFUNCTION(STRING_ESCAPE_SEMICOLON var str)

FUNCTION(STRING_PADDING var str length)
    SET(_ret "${str}")
    IF(${ARGN})
	SET(_padStr ${ARGN})
    ELSE(${ARGN})
	SET(_padStr " ")
    ENDIF(${ARGN})
    STRING(LENGTH "${str}" _strLen)
    STRING(LENGTH "${_padStr}" _padLen)
    WHILE( _strLen LESS length)
	SET(_ret "${_ret}${_padStr}")
	MATH(EXPR _strLen ${_strLen}+${_padLen})
    ENDWHILE( _strLen LESS length)
    SET(${var} "${_ret}" PARENT_SCOPE)
ENDFUNCTION(STRING_PADDING var str length)

FUNCTION(STRING_PREPEND var str)
    IF(${ARGC} GREATER 2)
	SET(_sep "${ARGV2}")
    ELSE(${ARGC} GREATER 2)
	SET(_sep "")
    ENDIF(${ARGC} GREATER 2)
    IF("${${var}}" STREQUAL "")
	SET(${var} "${str}" PARENT_SCOPE)
    ELSE("${${var}}" STREQUAL "")
	SET(${var} "${str}${_sep}${${var}}" PARENT_SCOPE)
    ENDIF("${${var}}" STREQUAL "")
ENDFUNCTION(STRING_PREPEND var str)

# Return (index of lefttmost non match character)
# Return _strLen if all characters matches regex
FUNCTION(STRING_LEFTMOST_NOTMATCH_INDEX var str regex)
    STRING(LENGTH "${str}" _strLen)
    SET(_index 0)
    SET(_ret ${_strLen})
    WHILE(_index LESS _strLen)
	STRING(SUBSTRING "${str}" ${_index} 1 _strCursor)
	#MESSAGE("***STRING_UNQUOTE: _i=${_index} _strCursor=${_strCursor}")
	IF(NOT "${_strCursor}" MATCHES "${regex}")
	    SET(_ret ${_index})
	    SET(_index ${_strLen})
	ENDIF(NOT "${_strCursor}" MATCHES "${regex}")

	MATH(EXPR _index ${_index}+1)
    ENDWHILE(_index LESS _strLen)
    SET(${var} ${_ret} PARENT_SCOPE)
ENDFUNCTION(STRING_LEFTMOST_NOTMATCH_INDEX var str)

# Return (index of rightmost non match character) +1
# Return 0 if all characters matches regex
#
FUNCTION(STRING_RIGHTMOST_NOTMATCH_INDEX var str regex)
    STRING(LENGTH "${str}" _strLen)
    MATH(EXPR _index ${_strLen})
    SET(_ret 0)
    WHILE(_index GREATER 0)
	MATH(EXPR _index_1 ${_index}-1)
	STRING(SUBSTRING "${str}" ${_index_1} 1 _strCursor)
	#MESSAGE("***STRING_UNQUOTE: _i=${_index} _strCursor=${_strCursor}")

	IF(NOT "${_strCursor}" MATCHES "${regex}")
	    SET(_ret ${_index})
	    SET(_index 0)
	ENDIF(NOT "${_strCursor}" MATCHES "${regex}")
	MATH(EXPR _index ${_index}-1)
    ENDWHILE(_index GREATER 0)
    SET(${var} ${_ret} PARENT_SCOPE)
ENDFUNCTION(STRING_RIGHTMOST_NOTMATCH_INDEX var str)

FUNCTION(STRING_TRIM var str)
    #_STRING_ESCAPE(_ret "${str}" ${ARGN})
    STRING_LEFTMOST_NOTMATCH_INDEX(_leftIndex "${str}" "[ \t\n\r]")
    STRING_RIGHTMOST_NOTMATCH_INDEX(_rightIndex "${str}" "[ \t\n\r]")
    # MESSAGE("_left=${_leftIndex} _rightIndex=${_rightIndex} str=|${str}|")
    MATH(EXPR _subLen ${_rightIndex}-${_leftIndex})
    SET(_NOUNQUOTE 0)
    FOREACH( _arg ${ARGN})
	IF(_arg STREQUAL "NOUNQUOTE")
	    SET(_NOUNQUOTE 1)
	ENDIF(_arg STREQUAL "NOUNQUOTE")
    ENDFOREACH( _arg ${ARGN})

    IF(_subLen GREATER 0)
	STRING(SUBSTRING "${str}" ${_leftIndex} ${_subLen} _ret)
	# IF _subLen > 1
	#   IF UNQUOTE; then unquote
	# Otherwise don't touch
	IF (_subLen GREATER 1)
	    IF(NOT _NOUNQUOTE)
		STRING_UNQUOTE(_ret "${_ret}")
	    ENDIF(NOT _NOUNQUOTE)
	ENDIF (_subLen GREATER 1)
    ELSE(_subLen GREATER 0)
	SET(_ret "")
    ENDIF(_subLen GREATER 0)
    SET(${var} "${_ret}" PARENT_SCOPE)

    # Unencoding
    #_STRING_UNESCAPE(${var} "${_ret}" ${ARGN})
ENDFUNCTION(STRING_TRIM var str)

# Internal function
# Nested Variable cannot be escaped here, as variable is already substituted
# at the time it passes to this macro.
FUNCTION(_STRING_ESCAPE var str)
    # ';' and '\' are tricky, need to be encoded.
    # '#' => '#H'
    # '\' => '#B'
    # ';' => '#S'
    # '$' => '#D'
    SET(_NOESCAPE_SEMICOLON "")
    SET(_ESCAPE_VARIABLE "")

    FOREACH(_arg ${ARGN})
	IF(${_arg} STREQUAL "NOESCAPE_SEMICOLON")
	    SET(_NOESCAPE_SEMICOLON "NOESCAPE_SEMICOLON")
	ELSEIF(${_arg} STREQUAL "ESCAPE_VARIABLE")
	    SET(_ESCAPE_VARIABLE "ESCAPE_VARIABLE")
	ENDIF(${_arg} STREQUAL "NOESCAPE_SEMICOLON")
    ENDFOREACH(_arg ${ARGN})

    STRING(REGEX REPLACE "#" "#H" _ret "${str}")

    STRING(REGEX REPLACE "\\\\" "#B" _ret "${_ret}")

    IF(NOT _ESCAPE_VARIABLE STREQUAL "")
	STRING(REGEX REPLACE "$" "#D" _ret "${_ret}")
    ENDIF(NOT _ESCAPE_VARIABLE STREQUAL "")

    IF(_NOESCAPE_SEMICOLON STREQUAL "")
	STRING(REGEX REPLACE ";" "#S" _ret "${_ret}")
    ENDIF(_NOESCAPE_SEMICOLON STREQUAL "")
    #MESSAGE("_STRING_ESCAPE:_ret=${_ret}")
    SET(${var} "${_ret}" PARENT_SCOPE)
ENDFUNCTION(_STRING_ESCAPE var str)

FUNCTION(_STRING_UNESCAPE var str)
    # '#B' => '\'
    # '#H' => '#'
    # '#D' => '$'
    # '#S' => ';'
    SET(_ESCAPE_VARIABLE "")
    SET(_NOESCAPE_SEMICOLON "")
    SET(_ret "${str}")
    FOREACH(_arg ${ARGN})
	IF(${_arg} STREQUAL "NOESCAPE_SEMICOLON")
	    SET(_NOESCAPE_SEMICOLON "NOESCAPE_SEMICOLON")
	ELSEIF(${_arg} STREQUAL "ESCAPE_VARIABLE")
	    SET(_ESCAPE_VARIABLE "ESCAPE_VARIABLE")
	    STRING(REGEX REPLACE "#D" "$" _ret "${_ret}")
	ENDIF(${_arg} STREQUAL "NOESCAPE_SEMICOLON")
    ENDFOREACH(_arg ${ARGN})
    #MESSAGE("###_STRING_UNESCAPE: var=${var} _ret=${_ret} _NOESCAPE_SEMICOLON=${_NOESCAPE_SEMICOLON} ESCAPE_VARIABLE=${_ESCAPE_VARIABLE}")

    STRING(REGEX REPLACE "#B" "\\\\" _ret "${_ret}")
    IF("${_NOESCAPE_SEMICOLON}" STREQUAL "")
	# ESCAPE_SEMICOLON
	STRING(REGEX REPLACE "#S" "\\\\;" _ret "${_ret}")
    ELSE("${_NOESCAPE_SEMICOLON}" STREQUAL "")
	# Don't ESCAPE_SEMICOLON
	STRING(REGEX REPLACE "#S" ";" _ret "${_ret}")
    ENDIF("${_NOESCAPE_SEMICOLON}" STREQUAL "")

    IF(NOT _ESCAPE_VARIABLE STREQUAL "")
	# '#D' => '$'
	STRING(REGEX REPLACE "#D" "$" _ret "${_ret}")
    ENDIF(NOT _ESCAPE_VARIABLE STREQUAL "")
    STRING(REGEX REPLACE "#H" "#" _ret "${_ret}")
    SET(${var} "${_ret}" PARENT_SCOPE)
    #MESSAGE("*** _STRING_UNESCAPE: ${var}=${${var}}")
ENDFUNCTION(_STRING_UNESCAPE var str)

FUNCTION(STRING_UNQUOTE var str)
    SET(_ret "${str}")
    STRING(LENGTH "${str}" _strLen)

    # IF _strLen > 1
    #   IF lCh and rCh are both "\""
    #      Remove _lCh and _rCh
    #   ELSEIF lCh and rCh are both "'"
    #      Remove _lCh and _rCh
    # Otherwise don't touch
    IF(_strLen GREATER 1)
	STRING(SUBSTRING "${str}" 0 1 _lCh)
	MATH(EXPR _strLen_1 ${_strLen}-1)
	MATH(EXPR _strLen_2 ${_strLen_1}-1)
	STRING(SUBSTRING "${str}" ${_strLen_1} 1 _rCh)
	#MESSAGE("_lCh=${_lCh} _rCh=${_rCh} _ret=|${_ret}|")
	IF("${_lCh}" STREQUAL "\"" AND "${_rCh}" STREQUAL "\"")
	    STRING(SUBSTRING "${_ret}" 1 ${_strLen_2} _ret)
	ELSEIF("${_lCh}" STREQUAL "'" AND "${_rCh}" STREQUAL "'")
	    STRING(SUBSTRING "${_ret}" 1 ${_strLen_2} _ret)
	ENDIF("${_lCh}" STREQUAL "\"" AND "${_rCh}" STREQUAL "\"")
    ENDIF (_strLen GREATER 1)
    SET(${var} "${_ret}" PARENT_SCOPE)
ENDFUNCTION(STRING_UNQUOTE var str)

MACRO(STRING_JOIN var delimiter)
    SET(_ret "")
    FOREACH(_str ${ARGN})
	STRING_APPEND(_ret "${_str}" "${delimiter}")
    ENDFOREACH(_str ${ARGN})
    SET(${var} "${_ret}")
ENDMACRO(STRING_JOIN var delimiter)

FUNCTION(STRING_FIND var str search_str)
    STRING(LENGTH "${str}" _str_len)
    STRING(LENGTH "${search_str}" _search_len)
    MATH(EXPR _str_end ${_str_len}-${_search_len}+1)

    SET(_index 0)
    SET(_str_remain "")
    SET(_result -1)
    WHILE(_index LESS _str_end)
	STRING(SUBSTRING "${str}" ${_index} ${_search_len} _str_window)
	IF(_str_window STREQUAL search_str)
	    SET(_result ${_index})
	    BREAK()
	ELSE(_str_window STREQUAL search_str)
	    MATH(EXPR _index ${_index}+1)
	ENDIF(_str_window STREQUAL search_str)
    ENDWHILE(_index LESS _str_end)
    SET(${var} ${_result} PARENT_SCOPE)
ENDFUNCTION(STRING_FIND var str search)

FUNCTION(STRING_SPLIT_2 var str_remain has_delimiter delimiter str)
    STRING_FIND(_index "${str}" "${delimiter}")
    IF(_index EQUAL -1)
	SET(${has_delimiter} "0" PARENT_SCOPE)
	SET(${var} "${str}" PARENT_SCOPE)
	SET(${str_remain} "" PARENT_SCOPE)
    ELSE(_index EQUAL -1)
	SET(${has_delimiter} "1" PARENT_SCOPE)
	STRING(SUBSTRING "${str}" 0 ${_index} _var)
	SET(${var} "${_var}" PARENT_SCOPE)

	STRING(LENGTH "${str}" _str_len)
	STRING(LENGTH "${delimiter}" _delimiter_len)
	MATH(EXPR _str_2_start ${_index}+${_delimiter_len})
	MATH(EXPR _str_2_len ${_str_len}-${_index}-${_delimiter_len})
	STRING(SUBSTRING "${str}" ${_str_2_start} ${_str_2_len} _str_remain)
	SET(${str_remain} "${_str_remain}" PARENT_SCOPE)
    ENDIF(_index EQUAL -1)
ENDFUNCTION(STRING_SPLIT_2 var str_remain has_delimiter delimiter str)

FUNCTION(STRING_SPLIT var delimiter str)
    #MESSAGE("***STRING_SPLIT: var=${var} str=${str}")
    SET(_max_tokens "")
    SET(_NOESCAPE_SEMICOLON "")
    SET(_ESCAPE_VARIABLE "")
    SET(_ALLOW_EMPTY "")
    FOREACH(_arg ${ARGN})
	IF(${_arg} STREQUAL "NOESCAPE_SEMICOLON")
	    SET(_NOESCAPE_SEMICOLON "NOESCAPE_SEMICOLON")
	ELSEIF(${_arg} STREQUAL "ESCAPE_VARIABLE")
	    SET(_ESCAPE_VARIABLE "ESCAPE_VARIABLE")
	ELSEIF(${_arg} STREQUAL "ALLOW_EMPTY")
	    SET(_ALLOW_EMPTY "ALLOW_EMPTY")
	ELSE(${_arg} STREQUAL "NOESCAPE_SEMICOLON")
	    SET(_max_tokens ${_arg})
	ENDIF(${_arg} STREQUAL "NOESCAPE_SEMICOLON")
    ENDFOREACH(_arg ${ARGN})

    IF(NOT _max_tokens)
	SET(_max_tokens -1)
    ENDIF(NOT _max_tokens)

    _STRING_ESCAPE(_str "${str}" ${_NOESCAPE_SEMICOLON} ${_ESCAPE_VARIABLE})
    _STRING_ESCAPE(_delimiter "${delimiter}" ${_NOESCAPE_SEMICOLON} ${_ESCAPE_VARIABLE})
    SET(_str_list "")
    SET(_token_count 1)

    WHILE(NOT _token_count EQUAL _max_tokens)
	STRING_SPLIT_2(_token _str _has_delimiter "${_delimiter}" "${_str}")
	#MESSAGE("_token_count=${_token_count} _max_tokens=${_max_tokens} _token=|${_token}| _str=${_str}")
	MATH(EXPR _token_count ${_token_count}+1)
	# Use length check to avoid the reserved word like "type"
	STRING(LENGTH "${_token}" _token_len)

	IF(_token_len GREATER 0 OR _ALLOW_EMPTY)
    	    LIST(APPEND _str_list "${_token}")
	ENDIF(_token_len GREATER 0 OR _ALLOW_EMPTY)
	IF(_has_delimiter EQUAL 0)
	    ## No more tokens
	    BREAK()
	ENDIF(_has_delimiter EQUAL 0)
    ENDWHILE(NOT _token_count EQUAL _max_tokens)

    IF(_has_delimiter EQUAL 1)
	LIST(APPEND _str_list "${_str}")
    ENDIF(_has_delimiter EQUAL 1)

    # Unencoding
    _STRING_UNESCAPE(_var "${_str_list}" ${_NOESCAPE_SEMICOLON} ${_ESCAPE_VARIABLE})
    #MESSAGE("***STRING_SPLIT: tokens=${${var}}")
    SET(${var} "${_var}" PARENT_SCOPE)
ENDFUNCTION(STRING_SPLIT var delimiter str)

