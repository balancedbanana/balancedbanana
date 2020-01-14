#include <SSLSocket.h>

void SSLSocket::connect(const std::string &address) {
    //socket.setPrivateKey()
}

void SSLSocket::listen(unsigned short port, const std::function<void(std::shared_ptr<SSLSocket>)> & callback) {
}

void SSLSocket::send(const const char * & msg) {
}

const char * SSLSocket::receive(const char* & data, const size_t & size) {
}

void SSLSocket::listen(unsigned short port, const std::function<void()> & callback) {
}