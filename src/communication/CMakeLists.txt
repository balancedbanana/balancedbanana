add_subdirectory(authenticator)
add_subdirectory(message)

add_library(communication STATIC
        Communicator.cpp
        CommunicatorListener.cpp
        message/AuthResultMessage.cpp
        message/Message.cpp
        message/ClientAuthMessage.cpp
        MessageProcessor.cpp message/HardwareDetailMessage.cpp)
target_link_libraries(communication PUBLIC  Net OpenSSL::SSL OpenSSL::Crypto)
