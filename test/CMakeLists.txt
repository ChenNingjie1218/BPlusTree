include_directories(${CMAKE_SOURCE_DIR}/test/include)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/proto)


enable_testing()

add_executable(
  unittest
  unitTest.cpp
)

target_link_libraries(
  unittest
  GTest::gtest_main
  GTest::gmock_main
)

include(GoogleTest)
# gtest_discover_tests(unittest)

find_package(Protobuf REQUIRED)

target_link_libraries(unittest bplustree_pb ${PROTOBUF_LIBRARIES})

target_link_libraries(unittest /usr/lib/x86_64-linux-gnu/libuuid.so)

add_executable(
  performancetest
  performanceTest.cpp
)
target_link_libraries(performancetest bplustree_pb ${PROTOBUF_LIBRARIES})
target_link_libraries(performancetest /usr/lib/x86_64-linux-gnu/libuuid.so)