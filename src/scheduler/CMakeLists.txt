cmake_minimum_required(VERSION 3.0)
add_subdirectory(queue)

add_executable(bbs Scheduler.cpp Worker.cpp Job.cpp IUser.cpp webserver/Webserver.cpp)
target_link_libraries(bbs Qt5::Core Qt5::Network Qt5::Sql communication Authenticator Message configfiles commandLineInterface)
set_target_properties(bbs PROPERTIES INSTALL_RPATH "\$ORIGIN/../lib")
install (TARGETS bbs Net RUNTIME DESTINATION bin LIBRARY DESTINATION lib)