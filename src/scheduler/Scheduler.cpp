#include <commandLineInterface/CommandLineProcessor.h>
#include <communication/CommunicatorListener.h>
#include <communication/messageProcessor/MessageProcessor.h>
#include <iostream>

using namespace balancedbanana::commandLineInterface;
using namespace balancedbanana::communication;

int main(int argc, char** argv) {
    CommandLineProcessor proc;
    // Wieso argc als Zeiger...
    auto task = proc.process(&argc, argv);
    std::cout << "Task Type" << task->getType() << "\n";
    CommunicatorListener listener(std::make_shared<MessageProcessor>());
    listener.listen([](std::shared_ptr<balancedbanana::communication::Message> message) {
        
    });
}