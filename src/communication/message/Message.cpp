#include <communication/message/Message.h>
#include <communication/message/ClientAuthMessage.h>
#include <communication/message/AuthResultMessage.h>
#include <communication/message/HardwareDetailMessage.h>
#include <communication/message/PublicKeyAuthMessage.h>
#include <communication/message/SnapshotMessage.h>
#include <communication/message/TaskMessage.h>
#include <communication/message/WorkerAuthMessage.h>
#include <Net/Http/V2/Stream.h>

using namespace balancedbanana::communication;

Message::Message(MessageType type) :
        type(type){
}

MessageType Message::GetType() const {
    return type;
}

std::string Message::serialize() const {
    std::stringstream stream;
    stream.write(reinterpret_cast<const char *>(&type), sizeof(MessageType));
    return stream.str();
}

std::shared_ptr<Message> Message::deserialize(const char *msg, uint32_t size) {
    /*uint32_t type;
    auto next = Net::Http::V2::GetUInt32(msg, type);
    switch (type)
    {
        case MessageType::AUTH_RESULT:


    case MessageType::CLIENT_AUTH: {
        std::string username(next);
        next += username.length() + 1;
        std::string password(next);
        next += password.length() + 1;
        return std::make_shared<ClientAuthMessage>(username, password, next);
    }
    default:
        break;
    }*/
    size_t iterator = 0;
    uint32_t type = serialization::extractUInt32(msg, iterator, size);
    switch(type) {
        case MessageType::AUTH_RESULT:
            return std::make_shared<AuthResultMessage>(msg, iterator, size);
        case MessageType::CLIENT_AUTH:
            return std::make_shared<ClientAuthMessage>(msg, iterator, size);
        case MessageType::HARDWARE_DETAIL:
            return std::make_shared<HardwareDetailMessage>(msg, iterator, size);
        case MessageType::PUBLIC_KEY_AUTH:
            return std::make_shared<PublicKeyAuthMessage>(msg, iterator, size);
        case MessageType::SNAPSHOT:
            return std::make_shared<SnapshotMessage>(msg, iterator, size);
        case MessageType::TASK:
            return std::make_shared<TaskMessage>(msg, iterator, size);
        case MessageType::WORKER_AUTH:
            return std::make_shared<WorkerAuthMessage>(msg, iterator, size);
        default:
            return nullptr;
    }
}

namespace balancedbanana::communication::serialization {

    void insert(std::ostream &stream, const std::string &value) {
        insert(stream, value.size());
        stream.write(value.data(), value.size());
    }

    void insert(std::ostream &stream, uint32_t value) {
        stream.write(reinterpret_cast<const char *>(&value), sizeof(uint32_t));
    }

    uint32_t extractUInt32(const char *data, size_t &iterator, size_t size) {
        if(data == nullptr) {
            throw std::invalid_argument("msg must not be null");
        }
        if (iterator + sizeof(uint32_t) > size) {
            throw std::invalid_argument("data is too small");
        }
        uint32_t value = *reinterpret_cast<const uint32_t *>(data + iterator);
        iterator += 4;
        return value;
    }

    std::string extractString(const char *data, size_t &iterator, size_t size) {
        if(data == nullptr) {
            throw std::invalid_argument("msg must not be null");
        }
        uint32_t stringsize = extractUInt32(data, iterator, size);
        if (iterator + stringsize > size) {
            throw std::invalid_argument("data is too small");
        }
        std::string value(data + iterator, stringsize);
        iterator += stringsize;
        return value;
    }

}