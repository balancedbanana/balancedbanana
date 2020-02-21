#include <communication/message/RespondToClientMessage.h>
#include <communication/message/Serialization.h>
#include <sstream>

namespace balancedbanana
{
namespace communication
{

RespondToClientMessage::RespondToClientMessage(const std::string &data, bool unblock)
    : Message(RESPOND_TO_CLIENT), data(data), unblock(unblock)
{
}

RespondToClientMessage::RespondToClientMessage(const char *data, size_t &iterator, size_t size)
    : Message(RESPOND_TO_CLIENT), data(""), unblock(false)
{
    this->data = serialization::extractString(data, iterator, size);
    this->unblock = serialization::extract<bool>(data, iterator, size);
}

std::string RespondToClientMessage::serialize() const {
    std::stringstream stream;
    stream << Message::serialize();
    serialization::insertString(stream, data);
    serialization::insert<bool>(stream, unblock);
    return stream.str();
}

void RespondToClientMessage::process(MessageProcessor &mp) const {
    mp.processRespondToClientMessage(*this);
}

const std::string &RespondToClientMessage::GetData() const
{
    return this->data;
}

const uint32_t RespondToClientMessage::getUnblock() const
{
    return this->unblock;
}

} // namespace communication
} // namespace balancedbanana