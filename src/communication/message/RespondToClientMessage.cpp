#include <communication/message/RespondToClientMessage.h>
#include <communication/message/Serialization.h>
#include <sstream>

namespace balancedbanana
{
namespace communication
{

RespondToClientMessage::RespondToClientMessage(const std::string &data, TaskType originalTaskType)
    : Message(RESPOND_TO_CLIENT), data(data), originalType((uint32_t)originalTaskType)
{
}

RespondToClientMessage::RespondToClientMessage(const char *data, size_t &iterator, size_t size)
    : Message(RESPOND_TO_CLIENT), data(""), originalType(0)
{
    this->data = serialization::extractString(data, iterator, size);
    this->originalType = serialization::extract<uint32_t>(data, iterator, size);
}

std::string RespondToClientMessage::serialize() const {
    std::stringstream stream;
    stream << Message::serialize();
    serialization::insertString(stream, data);
    serialization::insert<uint32_t>(stream, originalType);
    return stream.str();
}

void RespondToClientMessage::process(MessageProcessor &mp) const {
    mp.processRespondToClientMessage(*this);
}

const std::string &RespondToClientMessage::GetData() const
{
    return this->data;
}

const uint32_t RespondToClientMessage::getOriginalTaskType() const
{
    return this->originalType;
}

} // namespace communication
} // namespace balancedbanana