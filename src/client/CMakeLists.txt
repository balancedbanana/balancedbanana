add_executable(bbc Client.cpp)
target_link_libraries(bbc Authenticator communication configfiles commandLineInterface)
install (TARGETS bbc DESTINATION bin)