include(GoogleTest)

add_executable(AuthTest AuthTest.cpp)
target_link_libraries(AuthTest ${GTEST} Authenticator Message communication scheduler)
gtest_discover_tests(AuthTest)