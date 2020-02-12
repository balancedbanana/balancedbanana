#include <communication/message/PublicKeyAuthMessage.h>

#include <utility>

using namespace balancedbanana::communication;

PublicKeyAuthMessage::PublicKeyAuthMessage(std::string username, std::string usernamesignature) :
Message(PUBLIC_KEY_AUTH), username(std::move(username)), usernamesignature(std::move(usernamesignature)) {
}

PublicKeyAuthMessage::PublicKeyAuthMessage(const char *data, size_t &iterator, size_t size) :
Message(PUBLIC_KEY_AUTH), username(""), usernamesignature("") {
    username = serialization::extractString(data, iterator, size);
    usernamesignature = serialization::extractString(data, iterator, size);
}

void PublicKeyAuthMessage::process(MessageProcessor &mp) const {
    mp.processPublicKeyAuthMessage(*this);
}

std::string PublicKeyAuthMessage::serialize() const {
    std::stringstream stream;
    stream << Message::serialize();
    serialization::insertString(stream, username);
    serialization::insertString(stream, usernamesignature);
    return stream.str();
}

const std::string &PublicKeyAuthMessage::GetUserName() const {
    return username;
}

const std::string &PublicKeyAuthMessage::GetUserNameSignature() const {
    return usernamesignature;
}