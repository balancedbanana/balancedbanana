include(GoogleTest)

add_executable(queueTests QueueTest.cpp PriorityQueueTest.cpp)
target_link_libraries(queueTests ${GTEST} queue scheduler configfiles timedEvents)
gtest_discover_tests(queueTests)