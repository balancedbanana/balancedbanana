#include <communication/message/AuthResultMessage.h>

using namespace balancedbanana::communication;

AuthResultMessage::AuthResultMessage(uint32_t status) :
Message(MessageType::AUTH_RESULT), status(status){
}

AuthResultMessage::AuthResultMessage(const char *data, size_t &iterator, size_t length) :
Message(MessageType::AUTH_RESULT), status(1) {
    status = serialization::extract<uint32_t>(data, iterator, length);
}


uint32_t AuthResultMessage::getStatus() const {
    return status;
}

void AuthResultMessage::process(MessageProcessor& mp) const {
    mp.processAuthResultMessage(*this);
}

std::string AuthResultMessage::serialize() const {
    std::stringstream stream;
    stream << Message::serialize();
    serialization::insert<uint32_t>(stream, status);
    return stream.str();
}