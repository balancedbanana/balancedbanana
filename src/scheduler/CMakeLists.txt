cmake_minimum_required(VERSION 3.0)
add_subdirectory(queue)

#Lots of errors here, will not compile right now
add_subdirectory(clientRequests)

add_library(scheduler Worker.cpp Job.cpp User.cpp IUser.cpp httpserver/HttpServer.cpp smtpserver/SmtpServer.cpp SchedulerClientMP.cpp SchedulerWorkerMP.cpp)
target_link_libraries(scheduler PUBLIC database Qt5::Core Qt5::Network Qt5::Sql communication Authenticator Message configfiles commandLineInterface clientRequests)
add_executable(bbs Scheduler.cpp)
target_link_libraries(bbs ${WHOLEARCHIVEBEG} scheduler ${WHOLEARCHIVEEND})
set_target_properties(bbs PROPERTIES INSTALL_RPATH "\$ORIGIN/../lib")
install (TARGETS bbs Net RUNTIME DESTINATION bin LIBRARY DESTINATION lib)