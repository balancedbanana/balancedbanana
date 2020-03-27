include(GoogleTest)

enable_testing()
find_package(Python3 COMPONENTS Interpreter REQUIRED)
add_test(NAME testrunjob COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testrunjob.py WORKING_DIRECTORY ${RUNTIME_OUTPUT_DIRECTORY} )