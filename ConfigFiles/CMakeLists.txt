cmake_minimum_required(VERSION 3.0)

add_library(configfiles src/JobConfig.cpp src/SchedulerConfig.cpp)
set_target_properties(configfiles PROPERTIES CXX_STANDARD 17)
target_include_directories(configfiles PUBLIC include)