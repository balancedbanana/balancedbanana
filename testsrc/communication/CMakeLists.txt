add_subdirectory(authenticator)
include(GoogleTest)

add_executable(ConnectTest ConnectTest.cpp)
target_link_libraries(ConnectTest ${GTEST} communication Message)
gtest_discover_tests(ConnectTest)