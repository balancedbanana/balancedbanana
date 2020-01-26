cmake_minimum_required(VERSION 3.0)

add_library(commandLineInterface CommandLineProcessor.cpp Task.cpp)
target_link_libraries(commandLineInterface configfiles)