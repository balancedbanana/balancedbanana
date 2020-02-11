add_library(Authenticator STATIC Authenticator.cpp AuthHandler.cpp SSHAuthHandler.cpp)
target_include_directories(Authenticator PUBLIC ../..)
target_link_libraries(Authenticator Qt5::Core Qt5::Network OpenSSL::Crypto)