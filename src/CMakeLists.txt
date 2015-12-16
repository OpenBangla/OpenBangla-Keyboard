SET(SOURCES_CPP
    main.cpp)
SET(SOURCES_HPP
    log.h)

INCLUDE_DIRECTORIES(3rdparty im layout)

## At first, set platform independent things
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/layout)

## Platform depended things
IF(OS_LINUX)
  ## Platform is Linux
  INCLUDE_DIRECTORIES(im/ibus ${IBUS_INCLUDE_DIRS})
  LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/im/ibus)
  ADD_SUBDIRECTORY(im/ibus)
ENDIF(OS_LINUX)

## Include subdirectories
ADD_SUBDIRECTORY(layout)

ADD_EXECUTABLE(OpenBangla-Keyboard ${SOURCES_CPP} ${SOURCES_HPP})
TARGET_LINK_LIBRARIES(OpenBangla-Keyboard Layout)

IF(OS_LINUX)
  TARGET_LINK_LIBRARIES(OpenBangla-Keyboard IM_iBus ${IBUS_LIBRARIES})
ENDIF(OS_LINUX)
