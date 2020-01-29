#include <communication/message/AuthResultMessage.h>

using namespace balancedbanana::communication;

AuthResultMessage::AuthResultMessage(uint32_t status) :
Message(MessageType::AUTH_RESULT), status(status){
}

AuthResultMessage::AuthResultMessage(std::istream &stream) :
Message(MessageType::AUTH_RESULT), status(1) {
    stream.readsome((char *)&status, sizeof(uint32_t));
}

//Gibt den Authentfication status zurück 0 falls erfolgreich sonst ungleich 0
uint32_t AuthResultMessage::getStatus() {
    return status;
}

void AuthResultMessage::process(const std::shared_ptr<MessageProcessor>& mp) {
    mp->processAuthResultMessage(this);
}

void AuthResultMessage::serialize(std::ostream &stream) {
    Message::serialize(stream);
    stream << status;
    stream.flush();
}