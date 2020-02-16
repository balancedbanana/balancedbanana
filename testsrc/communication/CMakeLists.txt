add_subdirectory(authenticator)
include(GoogleTest)

add_executable(ConnectTest ConnectTest.cpp)
target_link_libraries(ConnectTest ${GTEST} communication Message scheduler)
gtest_discover_tests(ConnectTest)

add_executable(MessageTests message/MessageTests.cpp message/AuthResultMessageTests.cpp message/ClientAuthMessageTests.cpp)
target_link_libraries(MessageTests ${GTEST} communication)
gtest_discover_tests(MessageTests)