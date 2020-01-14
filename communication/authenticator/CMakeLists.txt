add_library(Authenticator STATIC src/Authenticator.cpp src/Authenticator.h)
target_link_libraries(Authenticator Qt5::Core Qt5::Network OpenSSL::Crypto)