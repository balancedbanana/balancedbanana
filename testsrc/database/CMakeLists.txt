include(GoogleTest)

# Executable for all the database tests
add_executable(databasetests WorkerGatewayTest.cpp UserGatewayTest.cpp JobGatewayTest.cpp)
target_link_libraries(databasetests configfiles database Qt5::Core ${GTEST})
gtest_discover_tests(databasetests)

