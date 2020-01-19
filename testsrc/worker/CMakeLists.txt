enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(workertests ContainerTest.cpp)
target_link_libraries(workertests GTest::GTest GTest::Main worker)
gtest_discover_tests(workertests)