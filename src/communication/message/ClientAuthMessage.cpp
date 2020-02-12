#include <communication/message/ClientAuthMessage.h>
#include <Net/Http/V2/Frame.h>

using namespace balancedbanana::communication;

using namespace serialization;

ClientAuthMessage::ClientAuthMessage(const std::string &username, const std::string &password,
        const std::string &pubkey) :
Message(MessageType::CLIENT_AUTH), username(username), password(password), publickey(pubkey) {
}

ClientAuthMessage::ClientAuthMessage(const char *data, size_t &iterator, size_t size) :
Message(MessageType::CLIENT_AUTH), username(""), password(""), publickey("") {
    username = extractString(data, iterator, size);
    password = extractString(data, iterator, size);
    publickey = extractString(data, iterator, size);
}

void ClientAuthMessage::process(MessageProcessor &mp) const {
    mp.processClientAuthMessage(*this);
}

std::string ClientAuthMessage::serialize() const {
    /*
    std::vector<uint8_t> buf(4);
    auto it = buf.begin();
    AddUInt32(type, it);
    std::stringstream out;
    out.write((const char*)buf.data(), 4) << username << '\0' << password << '\0' << publickey << '\0';
    return out.str();*/
    std::stringstream stream;
    stream << Message::serialize();
    insertString(stream, username);
    insertString(stream, password);
    insertString(stream, publickey);
    return stream.str();
}

const std::string &ClientAuthMessage::GetUsername() const {
    return username;
}

const std::string &ClientAuthMessage::GetPassword() const {
    return password;
}

const std::string &ClientAuthMessage::GetPublickey() const {
    return publickey;
}