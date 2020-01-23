#include <communication/Communicator.h>
#include <Net/TLSSocket.h>

using namespace balancedbanana::communication;

Communicator::Communicator(const std::shared_ptr<Net::Socket> &socket, const std::shared_ptr<MessageProcessor>& processor) {
    this->socket = socket;
    this->processor = processor;
    uint8_t buf[20];
    int count = this->socket->GetInputStream().Receive(buf, 20);
}

Communicator::Communicator(const std::string address, short port, const std::shared_ptr<MessageProcessor>& processor) {
    if(!(this->socket = Net::TLSSocket::Connect(address, port, false))) {
        throw std::runtime_error("Connection failed");
    }
    this->socket->GetOutputStream().Send((const uint8_t*)"Hallo Welt", 10);
    this->processor = processor;
}