#pragma once

class ClientMP : public MessageProcessor {
public:
	virtual void processMsgX(const std::shared_ptr<MsgX>& msg);

};
