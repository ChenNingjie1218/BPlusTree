include_directories(../include)
include_directories(../test)
add_executable(main Main.cpp)

# target_link_libraries(main test)

add_library(interface STATIC Interface.cpp)

target_link_libraries(main interface)