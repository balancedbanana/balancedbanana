add_subdirectory(queue)

add_executable(ObserverTests ObserverTests.cpp)
target_link_libraries(ObserverTests ${GTEST})
gtest_discover_tests(ObserverTests)