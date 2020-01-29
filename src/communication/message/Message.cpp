#include <communication/message/Message.h>

using namespace balancedbanana::communication;

Message::Message(MessageType type) :
type(type){
}

void Message::serialize(std::ostream &stream) {
    stream << type;
}