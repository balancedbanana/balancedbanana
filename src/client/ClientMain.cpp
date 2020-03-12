#include "client/Client.h"

using balancedbanana::client::Client;

int main(int argc, char** argv) {
    auto client = std::make_shared<Client>();
    return client->processCommandLineArguments(argc, argv).get();
}