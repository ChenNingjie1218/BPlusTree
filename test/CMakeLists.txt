# add_library(performance_test STATIC performanceTest.cpp)
# include_directories(${CMAKE_SOURCE_DIR}/test/include)
# include_directories(${CMAKE_SOURCE_DIR}/include)


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
gtest_discover_tests(unittest)

