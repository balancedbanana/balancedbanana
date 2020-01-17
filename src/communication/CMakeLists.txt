add_subdirectory(authenticator)

add_library(communication STATIC Communicator.cpp SSLSocket.cpp)
target_link_libraries(communication Qt5::Core Qt5::Network)