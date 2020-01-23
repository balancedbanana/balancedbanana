add_subdirectory(authenticator)

add_library(communication STATIC Communicator.cpp CommunicatorListener.cpp MessageProcessor.cpp SchedulerClientMP.cpp SchedulerWorkerMP.cpp)
target_link_libraries(communication Qt5::Core Qt5::Network Net OpenSSL::SSL OpenSSL::Crypto)