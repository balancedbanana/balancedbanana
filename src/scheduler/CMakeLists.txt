cmake_minimum_required(VERSION 3.0)
add_subdirectory(queue)

add_library(scheduler Worker.cpp Job.cpp IUser.cpp httpserver/HttpServer.cpp smtpserver/SmtpServer.cpp SchedulerClientMP.cpp SchedulerWorkerMP.cpp)
target_link_libraries(scheduler Qt5::Core Qt5::Network Qt5::Sql communication Authenticator Message configfiles commandLineInterface)
add_executable(bbs Scheduler.cpp)
target_link_libraries(bbs scheduler)
set_target_properties(bbs PROPERTIES INSTALL_RPATH "\$ORIGIN/../lib")
install (TARGETS bbs Net RUNTIME DESTINATION bin LIBRARY DESTINATION lib)