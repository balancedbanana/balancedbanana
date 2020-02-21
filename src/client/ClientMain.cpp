#include "client/Client.h"

using balancedbanana::client::Client;


#include "client/ClientMain.h"


int main(int argc, char** argv) {

    Client client;

    client.connectWithServer("::1", 8443);
    client.authenticateWithServer();
    client.processCommandLineArguments(argc, argv);

    // alternatively: use syncSetBlock un processCommandLineArguments
    block = client.specifiedBlock();

    uint32_t maxWait = 12 * 60 * 24 * 30; // one month excessive? maybe
    uint32_t hasWaited = 0;

    while (block) {
        if (hasWaited >= maxWait) {
            // safety: dont wait more than 30 days ...
            break;
        }
        // waiting, wakes up every 5 seconds to check if program can exit
        std::this_thread::sleep_for(std::chrono::seconds(5));
        ++hasWaited;
    }

    // authentication step
    /*
    auto com = std::make_shared<Communicator>("::1", 8443, std::make_shared<MessageProcessor>());
    balancedbanana::communication::authenticator::Authenticator auth(com);
    auth.authenticate(argv[1], argv[2]);
    */
}