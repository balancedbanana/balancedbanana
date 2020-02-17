enable_testing()
find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_executable(HttpTests HttpTests.cpp)
target_link_libraries(HttpTests ${GTEST} scheduler configfiles)
add_test(NAME HttpApiV1Test COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testapiv1.py WORKING_DIRECTORY ${RUNTIME_OUTPUT_DIRECTORY} )