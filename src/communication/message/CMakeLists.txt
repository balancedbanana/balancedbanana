add_library(Message STATIC Message.cpp ClientAuthMessage.cpp)
target_link_libraries(Message Qt5::Core Qt5::Network Net OpenSSL::SSL OpenSSL::Crypto)