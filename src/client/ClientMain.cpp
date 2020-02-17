#include "client/Client.h"

using balancedbanana::client::Client;


int main(int argc, char** argv) {

    Client client;

    client.connectWithServer("::1", 8443);
    client.authenticateWithServer();
    client.processCommandLineArguments(argc, argv);

    // authentication step
    /*
    auto com = std::make_shared<Communicator>("::1", 8443, std::make_shared<MessageProcessor>());
    balancedbanana::communication::authenticator::Authenticator auth(com);
    auth.authenticate(argv[1], argv[2]);
    */
}