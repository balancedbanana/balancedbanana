cmake_minimum_required(VERSION 3.0)

add_library(worker Container.cpp Docker.cpp )
set_target_properties(worker PROPERTIES CXX_STANDARD 17)
target_link_libraries(worker Qt5::Core Qt5::Network)