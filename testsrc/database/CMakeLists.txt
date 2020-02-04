include(GoogleTest)

add_executable(databasetests WorkerGatewayTest.cpp)
target_link_libraries(databasetests configfiles database Qt5::Core ${GTEST})
add_test(NAME    databasetests
        COMMAND databasetests)

