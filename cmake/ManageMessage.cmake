# - Manage the output and debug messages.
# This module has macros that control how many messages to be shown
# by defining the desire message level.
#
# Defined variables that represent verbose levels:
#   1: M_FATAL - Critical error,Should stop immediately
#   2: M_ERROR - Error that will Eventually fail
#   3: M_WARN  - General Warning.
#   4: M_OFF   - Optional functionalities are turned-off because requirement is not met.
#   5: M_INFO1 - Info/debug message
#   6: M_INFO2 - Info/debug message
#   7: M_INFO3 - Info/debug message
#
# Read following variable:
#   + MANAGE_MESSAGE_LEVEL: Message level in integer.
#     Messages with greater level will be suppressed.
#     Default is ${M_OFF}
#   + MANAGE_MESSAGE_LABELS: Labels that printed in front of the message for
#     corresponding message level.
#     Default is "[Fatal] ;[Error] ;[Warn] ;[Off] ;[Info1] ;[Info2] ;[Info3] ")
#
# Define following functions:
#   M_MSG(<level> <msg>)
#   - Surpress the message if level is higher than MANAGE_MESSAGE_LEVEL
#     Otherwise show the message.
#     * Parameters:
#       + level: level of the message.
#       + msg: Message to show.
#
#   M_OUT(<msg>)
#   - Output the message to stdout
#     In cmake-2.6.2, MESSAGE(...) output to stderr.
#     This function make it output to stdout instead.
#     * Parameters:
#       + msg: Message to show.

IF(DEFINED _MANAGE_MESSAGE_CMAKE_)
    RETURN()
ENDIF(DEFINED _MANAGE_MESSAGE_CMAKE_)
SET(_MANAGE_MESSAGE_CMAKE_ "DEFINED")

SET(M_FATAL 1)
SET(M_ERROR 2)
SET(M_WARN 3)
SET(M_OFF  4)
SET(M_INFO1 5)
SET(M_INFO2 6)
SET(M_INFO3 7)
IF(NOT DEFINED MANAGE_MESSAGE_LABELS)
    SET(MANAGE_MESSAGE_LABELS
	"[Fatal] ;[Error] ;[Warn] ;[Off] ;[Info1] ;[Info2] ;[Info3] ")
ENDIF(NOT DEFINED MANAGE_MESSAGE_LABELS)

SET(MANAGE_MESSAGE_LEVEL ${M_OFF} CACHE STRING "Message (Verbose) Level")

MACRO(M_MSG level)
    IF(NOT ${level} GREATER ${MANAGE_MESSAGE_LEVEL})
	MATH(EXPR _lvl ${level}-1)
	LIST(GET MANAGE_MESSAGE_LABELS ${_lvl} _label)
	IF(${level} EQUAL 1)
	    MESSAGE(FATAL_ERROR "${_label}${ARGN}")
	ELSE(${level} EQUAL 1)
	    MESSAGE("${_label}${ARGN}")
	ENDIF(${level} EQUAL 1)
    ENDIF(NOT ${level} GREATER ${MANAGE_MESSAGE_LEVEL})
ENDMACRO(M_MSG level)

FUNCTION(M_OUT)
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E echo "${ARGN}")
ENDFUNCTION(M_OUT)

