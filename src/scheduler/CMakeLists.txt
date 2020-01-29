cmake_minimum_required(VERSION 3.0)

add_library(scheduler SchedulerClientMP.cpp SchedulerWorkerMP.cpp Worker.cpp)
target_link_libraries(scheduler Qt5::Core Qt5::Network Qt5::Sql)