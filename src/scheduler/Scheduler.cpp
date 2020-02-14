#include <commandLineInterface/SchedulerCommandLineProcessor.h>
#include <communication/CommunicatorListener.h>
#include <scheduler/SchedulerClientMP.h>
#include <scheduler/httpserver/HttpServer.h>
#include <iostream>
#include <scheduler/Worker.h>

using namespace balancedbanana::commandLineInterface;
using namespace balancedbanana::communication;
using namespace balancedbanana::scheduler;

int main(int argc, char** argv) {
    SchedulerCommandLineProcessor proc;
    std::shared_ptr<Task> task = std::make_shared<Task>();
    if(proc.process(argc, argv, task)) {
        return -1;
    }
    std::cout << "Task Type" << task->getType() << "\n";
    // CommunicatorListener listener([](){ return std::make_shared<balancedbanana::communication::SchedulerClientMP>(); });
    // std::vector<Worker> worker;
    // listener.listen(task->getServerPort(), [&worker](std::shared_ptr<balancedbanana::communication::Communicator> com) {
    //     worker.emplace_back()
    // });
    
    HttpServer server;
    server.listen(task->getWebAPIIP(), task->getWebAPIPort());
    return 0;
}