#pragma once
#include "SSLSocket.h"


class Communicator {
private:
	SSLSocket* socket;

	MessageProcessor* processor;


	Worker* worker;


public:
	void listen(const std::function<void(std::shared_ptr<Message>)>& callback);

	send(const Message& message);


};