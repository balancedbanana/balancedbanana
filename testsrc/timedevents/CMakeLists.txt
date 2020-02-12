include(GoogleTest)

add_executable(timerTests TestTimer.cpp)
target_link_libraries(timerTests ${GTEST} timedEvents)
gtest_discover_tests(timerTests)