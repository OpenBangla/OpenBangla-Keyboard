SET(SOURCES_CPP
    main.cpp)
SET(SOURCES_HPP
    log.h)

## At first, set platform independent things
INCLUDE_DIRECTORIES(im 3rdparty layout)
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/layout)

## Include subdirectories
ADD_SUBDIRECTORY(im)
ADD_SUBDIRECTORY(3rdparty)
ADD_SUBDIRECTORY(layout)

## Platform depended things
IF(OS_LINUX)
  ## Platform is Linux
  INCLUDE_DIRECTORIES(${GLIB2_INCLUDE_DIRS} ${IBUS_INCLUDE_DIRS} im/ibus)
  LINK_DIRECTORIES(${GLIB2_LIBRARY_DIRS} ${IBUS_LIBRARY_DIRS}
                   ${CMAKE_CURRENT_BINARY_DIR}/im/ibus)
ENDIF(OS_LINUX)

ADD_EXECUTABLE(OpenBangla-Keyboard ${SOURCES_CPP} ${SOURCES_HPP})
TARGET_LINK_LIBRARIES(OpenBangla-Keyboard Layout)

IF(OS_LINUX)
  TARGET_LINK_LIBRARIES(OpenBangla-Keyboard ${GLIB2_LIBRARIES} ${IBUS_LIBRARIES} IM_iBus)
ENDIF(OS_LINUX)
