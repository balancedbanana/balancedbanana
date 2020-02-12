add_subdirectory(authenticator)
add_subdirectory(message)

add_library(communication STATIC
        Communicator.cpp
        CommunicatorListener.cpp
        message/AuthResultMessage.cpp
        message/Message.cpp
        message/ClientAuthMessage.cpp
        message/HardwareDetailMessage.cpp
        MessageProcessor.cpp
        Task.cpp
        )
target_link_libraries(communication PUBLIC  Net OpenSSL::SSL OpenSSL::Crypto)
