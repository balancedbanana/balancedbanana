#include <communication/message/TaskMessage.h>
#include <communication/message/Serialization.h>
#include <communication/MessageProcessor.h>
#include <sstream>

using namespace balancedbanana::communication;

using balancedbanana::communication::TaskMessage;

TaskMessage::TaskMessage(const Task &task) :
Message(TASK), task(){

}

TaskMessage::TaskMessage(const char *data, size_t &iterator, size_t size) :
Message(TASK), task(serialization::extractString(data, iterator, size)){
}

void TaskMessage::process(MessageProcessor &mp) const {
    mp.processTaskMessage(*this);
}

std::string TaskMessage::serialize() const {
    std::stringstream stream;
    stream << Message::serialize();
    serialization::insertString(stream, task.serialize());
    return stream.str();
}

const Task &TaskMessage::GetTask() const {
    return task;
}