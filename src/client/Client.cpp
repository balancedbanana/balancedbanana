#include <commandLineInterface/CommandLineProcessor.h>
#include <communication/Communicator.h>
#include <communication/messageProcessor/MessageProcessor.h>
#include <iostream>

using namespace balancedbanana::commandLineInterface;
using namespace balancedbanana::communication;

int main(int argc, char** argv) {
    CommandLineProcessor proc;
    // Wieso argc als Zeiger...
    auto task = proc.process(&argc, argv);
    std::cout << "Task Type" << task->getType() << "\n";
    Communicator com("::1", 8443, std::make_shared<MessageProcessor>());
}