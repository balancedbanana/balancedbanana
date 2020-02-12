add_library(Message STATIC
        AuthResultMessage.cpp
        ClientAuthMessage.cpp
        HardwareDetailMessage.cpp
        Message.cpp
        PublicKeyAuthMessage.cpp
        Serialization.cpp
        SnapshotMessage.cpp
        WorkerAuthMessage.cpp
        TaskMessage.cpp)
target_link_libraries(Message Qt5::Core Qt5::Network Net OpenSSL::SSL OpenSSL::Crypto)