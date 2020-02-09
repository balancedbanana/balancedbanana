include(GoogleTest)

add_executable(AuthTest AuthTest.cpp)
target_link_libraries(AuthTest ${GTEST} communication Message Authenticator)
gtest_discover_tests(AuthTest)