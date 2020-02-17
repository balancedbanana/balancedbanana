#pragma once

#include <communication/MessageProcessor.h>

class WorkerMP : public MessageProcessor {
public:
	virtual void processMsgY(const std::shared_ptr<MsgY>& msg);

};