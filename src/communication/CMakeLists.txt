add_subdirectory(authenticator)

# add_library(communication STATIC src/Communicator.cpp include/Communicator.h src/SSLSocket.cpp include/SSLSocket.h)
# target_include_directories(communication PUBLIC include Qt5::Core Qt5::Network)

add_executable(communicationTest testsrc/main.cpp src/Communicator.cpp Communicator.h src/SSLSocket.cpp SSLSocket.h)
target_include_directories(communicationTest PUBLIC include)
target_link_libraries(communicationTest Qt5::Core Qt5::Network OpenSSL::Crypto)
