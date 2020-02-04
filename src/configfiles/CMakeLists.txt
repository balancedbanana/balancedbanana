cmake_minimum_required(VERSION 3.0)

add_library(configfiles JobConfig.cpp SchedulerConfig.cpp Priority.cpp)
if(${NEEDS_FS})
    target_link_libraries(configfiles PUBLIC stdc++fs)
endif()
