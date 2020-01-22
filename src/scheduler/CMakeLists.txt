cmake_minimum_required(VERSION 3.0)
add_subdirectory(queue)

add_library(scheduler Worker.cpp)
target_link_libraries(scheduler Qt5::Core Qt5::Network Qt5::Sql)