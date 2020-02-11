#include <commandLineInterface/CommandLineProcessor.h>
#include <communication/CommunicatorListener.h>
#include <communication/messageProcessor/SchedulerClientMP.h>
#include <iostream>

using namespace balancedbanana::commandLineInterface;
using namespace balancedbanana::communication;

void RunWebServer();

int main(int argc, char** argv) {
    // CommandLineProcessor proc;
    // // Wieso argc als Zeiger...
    // auto task = proc.process(argc, argv);
    // std::cout << "Task Type" << task->GetType() << "\n";
    // CommunicatorListener listener([](){ return std::make_shared<balancedbanana::communication::SchedulerClientMP>(); });
    // listener.listen([](std::shared_ptr<balancedbanana::communication::Communicator> com) {
        
    // });
    RunWebServer();
}