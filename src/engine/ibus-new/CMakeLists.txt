include_directories(../riti/include)
add_executable(ibus-openbangla main.c)
target_link_libraries(ibus-openbangla riti dl pthread)
