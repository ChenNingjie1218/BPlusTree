include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/test/include)
include_directories(${CMAKE_SOURCE_DIR}/proto)
add_executable(main Main.cpp)
target_link_libraries(main performance_test)


add_library(interface STATIC Interface.cpp)
target_link_libraries(main interface)


find_package(Protobuf REQUIRED)

target_link_libraries(main bplustree_pb ${PROTOBUF_LIBRARIES})

target_link_libraries(main /usr/lib/x86_64-linux-gnu/libuuid.so)

