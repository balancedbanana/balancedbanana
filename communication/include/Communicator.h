#pragma once
#include "SSLSocket.h"

class Communicator {
public:
    void Connect();

    void listen(const std::function<void(std::shared_ptr<Message>)> & callback);

    void send(const Message & message);
};
