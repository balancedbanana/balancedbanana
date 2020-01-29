#include <communication/message/ClientAuthMessage.h>

using namespace balancedbanana::communication;

ClientAuthMessage::ClientAuthMessage(const std::string& username, const std::string& password,
        const std::string& pubkey) :
Message(MessageType::CLIENT_AUTH), username(username), password(password), publickey(pubkey) {
}

ClientAuthMessage::ClientAuthMessage(std::istream &stream) :
Message(MessageType::CLIENT_AUTH), username(""), password(""), publickey("") {
    uint64_t size;
    stream >> size;
    stream.readsome((char*)&username, size);
    stream >> size;
    stream.readsome((char*)&password, size);
    stream >> size;
    stream.readsome((char*)&publickey, size);
}

void ClientAuthMessage::process(const std::shared_ptr<MessageProcessor>& mp) {
    mp->processClientAuthMessage(this);
}

void ClientAuthMessage::serialize(std::ostream &stream) {
    Message::serialize(stream);
    stream << (uint64_t) username.size() << username;
    stream << (uint64_t) password.size() << password;
    stream << (uint64_t) publickey.size() << publickey;
    stream.flush();
}