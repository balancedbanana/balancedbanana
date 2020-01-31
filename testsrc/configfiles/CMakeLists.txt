cmake_minimum_required(VERSION 3.10)
include(GoogleTest)
find_package(GTest REQUIRED)
enable_testing()

add_executable(jobconfigtests JobConfigTests.cpp)
set_target_properties(jobconfigtests PROPERTIES CXX_STANDARD 17)
target_link_libraries(jobconfigtests PUBLIC configfiles GTest::GTest GTest::Main)
gtest_discover_tests(jobconfigtests)