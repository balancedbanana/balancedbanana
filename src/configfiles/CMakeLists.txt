cmake_minimum_required(VERSION 3.0)

add_library(configfiles JobConfig.cpp SchedulerConfig.cpp)
set_target_properties(configfiles PROPERTIES CXX_STANDARD 17)