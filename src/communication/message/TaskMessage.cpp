#pragma once

#include <communication/message/TaskMessage.h>

using namespace balancedbanana::communication;

using balancedbanana::communication::TaskMessage;


TaskMessage::TaskMessage(const Task& task)
{
    this->task = task;
}


void TaskMessage::process(const MessageProcessor* & msgProcessor)
{
    // TODO: msgProcessor->processTaskMessage(this);
}


std::string TaskMessage::serialize()
{
    // TODO: implementthis
}