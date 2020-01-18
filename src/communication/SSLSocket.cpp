#include <communication/SSLSocket.h>

void SSLSocket::connect(const std::string &address) {
    //socket.setPrivateKey()
    // socket.connectToHostEncrypted(QString(address.data()), 6999);
}

void SSLSocket::listen(unsigned short port, const std::function<void(std::shared_ptr<SSLSocket>)> & callback) {
    // socket.startServerEncryption();
    // socket.bind()
}

void SSLSocket::send(const char * msg) {
}

const char * SSLSocket::receive(const char* data, const size_t & size) {
}

void SSLSocket::listen(unsigned short port, const std::function<void()> & callback) {
}