include(GoogleTest)

add_executable(databasetests WorkerGatewayTest.cpp)
target_link_libraries(databasetests configfiles database Qt5::Core ${GTEST})
gtest_discover_tests(databasetests)

