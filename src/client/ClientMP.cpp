#include "client/ClientMP.h"

#include <iostream>
#include "client/ClientMain.h"

using namespace balancedbanana::client;

void ClientMP::processRespondToClientMessage(const RespondToClientMessage& msg)
{
    // Received when the client made a request from the server
    std::string responseMessage = msg.GetData();
    bool unblock = msg.getUnblock();

    std::cout << responseMessage << std::endl;

    if (unblock) {
        // synchronize with client and tell it to unblock
        syncSetBlock(false);
    }
}