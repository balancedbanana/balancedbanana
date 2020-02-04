#include <communication/Communicator.h>
#include <Net/TLSSocket.h>
#include <Net/Http/V2/Stream.h>
#include <thread>

using namespace balancedbanana::communication;

Communicator::Communicator(const std::shared_ptr<Net::Socket> &socket, const std::shared_ptr<MessageProcessor>& processor) {
    this->socket = socket;
    this->processor = processor;
    // uint8_t buf[20];
    // size_t count = this->socket->GetInputStream().Receive(buf, 20);
    std::thread([this, socket=socket, processor=processor]() {
        auto in = socket->GetInputStream();
        std::vector<uint8_t> buf(500);
        while (true) {
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
    }).detach();
}

Communicator::Communicator(const std::string address, short port, const std::shared_ptr<MessageProcessor>& processor) {
    if(!(this->socket = Net::TLSSocket::Connect(address, port, false))) {
        throw std::runtime_error("Connection failed");
    }
    // this->socket->GetOutputStream().Send((const uint8_t*)"Hallo Welt", 10);
    this->processor = processor;
    std::thread([this, socket = this->socket, processor = this->processor]() {
        auto in = socket->GetInputStream();
        std::vector<uint8_t> buf(500);
        while (true) {
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
    }).detach();
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
