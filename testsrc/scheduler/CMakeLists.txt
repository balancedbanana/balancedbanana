add_subdirectory(clientRequests)
add_subdirectory(httpserver)
add_subdirectory(smtpserver)
add_subdirectory(queue)

add_executable(ObserverTests ObserverTests.cpp)
target_link_libraries(ObserverTests ${GTEST})
gtest_discover_tests(ObserverTests)


