add_subdirectory(authenticator)
include(GoogleTest)

add_executable(ConnectTest ConnectTest.cpp)
target_link_libraries(ConnectTest ${GTEST} communication Message)
gtest_discover_tests(ConnectTest)

add_executable(MessageTests
        message/AuthResultMessageTests.cpp
        message/ClientAuthMessageTests.cpp
        message/HardwareDetailMessageTests.cpp
        message/MessageTests.cpp
        message/PublicKeyAuthMessageTests.cpp
        message/TaskMessageTests.cpp
        message/WorkerAuthMessageTests.cpp
        message/MessagesProcessTests.cpp
        message/TaskResponseMessageTests.cpp
        message/WorkerLoadResponseMessageTests.cpp
        message/RespondToClientMessageTests.cpp)
target_link_libraries(MessageTests ${GTEST} communication scheduler)
gtest_discover_tests(MessageTests)