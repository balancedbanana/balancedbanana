include(GoogleTest)

add_executable(SmtpTests SmtpTest.cpp)
target_link_libraries(SmtpTests ${GTEST} scheduler configfiles)
gtest_discover_tests(SmtpTests)