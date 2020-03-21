include(GoogleTest)

# Executable for all the database tests
add_executable(databasetests WorkerGatewayTests.cpp UserGatewayTests.cpp JobGatewayTests.cpp UtilitiesTests.cpp
        FactoryTests.cpp RepositoryTests.cpp)
target_link_libraries(databasetests ${WHOLEARCHIVEBEG} communication scheduler configfiles database ${WHOLEARCHIVEEND} Qt5::Core ${GTEST})
gtest_discover_tests(databasetests)

