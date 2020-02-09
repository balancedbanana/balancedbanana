#include <communication/Communicator.h>
#include <Net/TLSSocket.h>
#include <Net/Http/V2/Stream.h>
#include <thread>
#include <stdexcept>

using namespace balancedbanana::communication;

Communicator::Communicator(const std::shared_ptr<Net::Socket> &socket, const std::shared_ptr<MessageProcessor>& processor) {
    if(socket == nullptr || processor == nullptr) {
        throw std::invalid_argument("socket and proccessor must be not null");
    }
    keeprunning = std::make_shared<std::atomic_bool>(true);
    this->socket = socket;
    msghandlerthread = std::thread(std::bind(&Communicator::msghandler, socket, processor, keeprunning));
}

Communicator::Communicator(const std::string address, short port, const std::shared_ptr<MessageProcessor>& processor) {
    if(!processor) {
        throw std::invalid_argument("proccessor must be not null");
    }
    if(!(this->socket = Net::TLSSocket::Connect(address, port, false))) {
        throw std::runtime_error("Connection failed");
    }
    keeprunning = std::make_shared<std::atomic_bool>(true);
    msghandlerthread = std::thread(std::bind(&Communicator::msghandler, socket, processor, keeprunning));
}

void Communicator::msghandler(std::shared_ptr<Net::Socket> socket, std::shared_ptr<MessageProcessor> processor, std::shared_ptr<std::atomic_bool> keeprunning) {
    auto in = socket->GetInputStream();
    std::vector<uint8_t> buf(500);
    auto& stop = *keeprunning;
    while (stop.load()) {
        if(!in.ReceiveAll(buf.data(), 4)) {
            return;
        }
        uint32_t length;
        auto data = Net::Http::V2::GetUInt32(buf.data(), length);
        if(!in.ReceiveAll(data, length)) {
            return;
        }
        processor->process(Message::deserialize((char*)data, length));
    }
}

Communicator::~Communicator() {
    if(msghandlerthread.joinable()) {
        *keeprunning = false;
        msghandlerthread.join();
    }
}

void Communicator::send(const Message & message) {
    std::string str = ((Message&)message).serialize();
    std::vector<uint8_t> buf(4);
    auto it = buf.begin();
    AddUInt32((uint32_t)str.length(), it);
    auto out = socket->GetOutputStream();
    out.SendAll(buf);
    out.SendAll((uint8_t*)str.data(), str.length());
}

void Communicator::detach() {
    msghandlerthread.detach();
}