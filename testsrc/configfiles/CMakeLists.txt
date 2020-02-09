include(GoogleTest)

add_executable(configfilestests JobConfigTests.cpp PriorityTests.cpp ApplicationConfigTests.cpp)
target_link_libraries(configfilestests PUBLIC configfiles ${GTEST})
gtest_discover_tests(configfilestests)