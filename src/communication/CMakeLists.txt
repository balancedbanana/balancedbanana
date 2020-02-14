add_subdirectory(authenticator)
add_subdirectory(message)

add_library(communication STATIC
        Communicator.cpp
        CommunicatorListener.cpp
        MessageProcessor.cpp
        ../data/Task.cpp
        )
target_link_libraries(communication PUBLIC Message Authenticator configfiles)
