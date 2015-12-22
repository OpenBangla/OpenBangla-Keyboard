set(SOURCES_CPP
    main.cpp)
set(SOURCES_HPP
    log.h)

include_directories(3rdparty im layout)

## At first, set platform independent things
link_directories(${CMAKE_CURRENT_BINARY_DIR}/layout)

## Platform depended things
if(OS_LINUX)
  ## Platform is Linux
  include_directories(im/ibus ${IBUS_INCLUDE_DIRS})
  link_directories(${CMAKE_CURRENT_BINARY_DIR}/im/ibus)
  add_subdirectory(im/ibus)
endif(OS_LINUX)

## Include subdirectories
add_subdirectory(layout)

add_executable(OpenBangla-Keyboard ${SOURCES_CPP} ${SOURCES_HPP})
target_link_libraries(OpenBangla-Keyboard Layout)

if(OS_LINUX)
  target_link_libraries(OpenBangla-Keyboard IM_iBus ${IBUS_LIBRARIES})
endif(OS_LINUX)
