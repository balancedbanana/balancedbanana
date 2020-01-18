cmake_minimum_required(VERSION 3.0)

add_library(database Factory.cpp Gateway.cpp)
set_target_properties(database PROPERTIES CXX_STANDARD 17)
target_link_libraries(database Qt5::Core Qt5::Network Qt5::Sql)