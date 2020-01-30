include(GoogleTest)

add_executable(gatewaytests GatewayTest.cpp)
target_link_libraries(gatewaytests configfiles database Qt5::Core ${GTEST})


add_test(NAME    gatewaytests
        COMMAND gatewaytests)