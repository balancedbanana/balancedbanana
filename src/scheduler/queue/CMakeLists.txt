add_library(queue Queue.cpp PriorityQueue.cpp)
target_link_libraries(queue Qt5::Core Qt5::Network Qt5::Sql)