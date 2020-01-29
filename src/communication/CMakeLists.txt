add_subdirectory(authenticator)

add_library(communication STATIC
        Communicator.cpp
        message/AuthResultMessage.cpp
        message/Message.cpp
        message/ClientAuthMessage.cpp
        MessageProcessor.cpp
        SSLSocket.cpp)
target_link_libraries(communication Qt5::Core Qt5::Network)