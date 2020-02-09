include(GoogleTest)

add_executable(queueTests QueueTest.cpp)
target_link_libraries(queueTests ${GTEST} queue configfiles)
gtest_discover_tests(queueTests)