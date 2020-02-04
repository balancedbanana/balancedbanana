include(GoogleTest)

add_executable(jobconfigtests JobConfigTests.cpp)
target_link_libraries(jobconfigtests PUBLIC configfiles ${GTEST})
gtest_discover_tests(jobconfigtests)