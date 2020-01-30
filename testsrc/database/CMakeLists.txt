include(GoogleTest)

add_executable(gatewaytests GatewayTest.cpp)


add_dependencies(gatewaytests ${GTEST})
include_directories(${source_dir}/include)
target_link_libraries(gatewaytests ${binary_dir}/libgtest.a)
target_link_libraries(gatewaytests ${binary_dir}/libgtest_main.a)
target_link_libraries(gatewaytests ${GTEST} gateway)
enable_testing()
add_test(NAME    gatewaytests
        COMMAND gatewaytests)