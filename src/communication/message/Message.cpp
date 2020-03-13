#include <communication/message/Message.h>
#include <communication/message/ClientAuthMessage.h>
#include <communication/message/AuthResultMessage.h>
#include <communication/message/HardwareDetailMessage.h>
#include <communication/message/PublicKeyAuthMessage.h>
#include <communication/message/TaskMessage.h>
#include <communication/message/TaskResponseMessage.h>
#include <communication/message/WorkerAuthMessage.h>
#include <communication/message/WorkerLoadRequestMessage.h>
#include <communication/message/WorkerLoadResponseMessage.h>
#include <communication/message/Serialization.h>
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
    serialization::insert<uint32_t>(stream, type);
    return stream.str();
}

std::shared_ptr<Message> Message::deserialize(const char *msg, uint32_t size) {
    size_t iterator = 0;
    uint32_t type = serialization::extract<uint32_t>(msg, iterator, size);
    switch(type) {
        case MessageType::AUTH_RESULT:
            return std::make_shared<AuthResultMessage>(msg, iterator, size);
        case MessageType::CLIENT_AUTH:
            return std::make_shared<ClientAuthMessage>(msg, iterator, size);
        case MessageType::HARDWARE_DETAIL:
            return std::make_shared<HardwareDetailMessage>(msg, iterator, size);
        case MessageType::JOB_STATUS:
            return std::make_shared<TaskResponseMessage>(msg, iterator, size);
        case MessageType::PUBLIC_KEY_AUTH:
            return std::make_shared<PublicKeyAuthMessage>(msg, iterator, size);
        case MessageType::TASK:
            return std::make_shared<TaskMessage>(msg, iterator, size);
        case MessageType::WORKER_AUTH:
            return std::make_shared<WorkerAuthMessage>(msg, iterator, size);
        case MessageType::WORKERLOADREQUEST:
            return std::make_shared<WorkerLoadRequestMessage>(msg, iterator, size);
        case MessageType::WORKERLOADRESPONSE:
            return std::make_shared<WorkerLoadResponseMessage>(msg, iterator, size);
        default:
            return nullptr;
    }
}
