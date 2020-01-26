cmake_minimum_required(VERSION 3.0)

add_executable(bbs Scheduler.cpp Worker.cpp IUser.cpp)
target_link_libraries(bbs Qt5::Core Qt5::Network Qt5::Sql communication Authenticator Message configfiles commandLineInterface)