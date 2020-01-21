include(GoogleTest)

add_executable(workertests ContainerTest.cpp)
target_link_libraries(workertests ${GTEST} worker)
gtest_discover_tests(workertests)