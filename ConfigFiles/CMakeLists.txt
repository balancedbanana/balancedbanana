cmake_minimum_required(VERSION 3.0)

add_library(configfiles src/JobConfig.cpp src/SchedulerConfig.cpp)
target_include_directories(configfiles PUBLIC include)