#pragma once


class MessageProcessor {
private:
	SSLSocket*;

	Communicator*;


public:
	virtual void process(const std::shared_ptr<Message>& msg);

	virtual void processMsgX(const std::shared_ptr<MsgX>& msg);

	virtual void processMsgY(const std::shared_ptr<MsgY>& msg);


private:
	virtual void handleInvalidMessage(const std::shared_ptr<Message>& msg);

};
