#include <commandLineInterface/CommandLineProcessor.h>
#include <communication/Communicator.h>
#include <communication/messageProcessor/MessageProcessor.h>
#include <communication/authenticator/Authenticator.h>
#include <iostream>

using namespace balancedbanana::commandLineInterface;
using namespace balancedbanana::communication;
int printcer();

int main(int argc, char** argv) {
    printcer();
    CommandLineProcessor proc;
    // Wieso argc als Zeiger...
    auto task = proc.process(&argc, argv);
    std::cout << "Task Type" << task->getType() << "\n";
    auto com = std::make_shared<Communicator>("::1", 8443, std::make_shared<MessageProcessor>());
    balancedbanana::communication::authenticator::Authenticator auth(com);
    auth.authenticate(argv[1], argv[2]);
}