cmake_minimum_required(VERSION 3.0)

add_library(scheduler Worker.cpp)
set_target_properties(scheduler PROPERTIES CXX_STANDARD 17)
target_link_libraries(scheduler Qt5::Core Qt5::Sql)