cmake_minimum_required(VERSION 3.0)

add_library(database Factory.cpp Gateway.cpp)
target_link_libraries(database Qt5::Core Qt5::Network Qt5::Sql)