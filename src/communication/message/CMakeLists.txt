add_library(Serialization Serialization.cpp)

add_library(Message STATIC
        AuthResultMessage.cpp
        ClientAuthMessage.cpp
        HardwareDetailMessage.cpp
        Message.cpp
        PublicKeyAuthMessage.cpp
        SnapshotMessage.cpp
        WorkerAuthMessage.cpp
        TaskMessage.cpp)
target_link_libraries(Message PRIVATE Qt5::Core Qt5::Network)
target_link_libraries(Message PUBLIC Net OpenSSL::SSL OpenSSL::Crypto communication Serialization configfiles)