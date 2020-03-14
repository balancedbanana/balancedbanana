#include <scheduler/httpserver/HttpServer.h>
#include <scheduler/SchedulerWorkerMP.h>
#include <communication/CommunicatorListener.h>
#include <iostream>

using namespace balancedbanana::scheduler;
using namespace balancedbanana::communication;

struct TestMP : SchedulerWorkerMP {
    TestMP() : SchedulerWorkerMP([](const std::string& name, const std::string& pubkey) -> std::shared_ptr<Worker> {
        throw std::runtime_error("How the hell");
    }, [](const std::string &worker) -> std::shared_ptr<balancedbanana::scheduler::Worker> {
        throw std::runtime_error("How the hell");
    }, [](auto) -> std::shared_ptr<Job> {
        throw std::runtime_error("How the hell");
    }) {}

    std::shared_ptr<Communicator> wcom;
    void handleInvalidMessage(const Message &msg) override {
        std::cout << "Invalid Message Ignore it\n";
    }

    void processWorkerLoadRequestMessage(const WorkerLoadRequestMessage &msg) override {
        WorkerLoadResponseMessage resp(1, 2, 3, 4, 5, 6, 7);
        wcom->send(resp);
    }
    
};

int main() {
    std::vector<std::shared_ptr<Worker>> worker;
    auto testmp = std::make_shared<TestMP>();
    auto listener = std::make_shared<CommunicatorListener>([testmp](){
        return testmp;
    });
    listener->listen(2435, [listener, &worker, testmp](std::shared_ptr<balancedbanana::communication::Communicator> com) {
        worker.emplace_back(std::make_shared<Worker>(0, "name", "key", balancedbanana::database::Specs{"GNU/Linux", 0, 0}))->setCommunicator(com);
        testmp->wcom = com;
        com->detach();
    });
    HttpServer server = HttpServer([&]() -> std::vector<std::shared_ptr<Worker>> {
        return worker;
    }, [](int workerid) -> std::vector<int> {
        return { 0, 2, 5, 7 }; 
    }, [](int userid) -> std::vector<int> {
        return { 0, 5, 7 }; 
    }, [](int hours) -> std::vector<int> {
        return { 0, 2 }; 
    }, [](int jobid) -> std::shared_ptr<Job> {
        // TODO Rakan: Ich habe allocated_space mit allocated_osIdentifier ersetzt
        return std::make_shared<Job>(jobid, QDateTime(QDate(2020, 1, 30), QTime(20, 10, 23, 0)), QDateTime(QDate(2020, 1, 30), QTime(22, 10, 23, 0)), QDateTime(QDate(2020, 1, 30), QTime(23, 10, 23, 0)), 100, 10, "", "Steve", 2, std::make_shared<User>(100, "Steve", ""), std::make_shared<balancedbanana::configfiles::JobConfig>(), balancedbanana::database::scheduled, std::make_shared<balancedbanana::database::job_result>());
    });
    auto com = std::make_shared<Communicator>("localhost", 2435, testmp);
    server.listen("localhost", 8234);
    com->detach();
    std::string cmd;
    while(1) {
        std::cin >> cmd;
        if(cmd == "stop") {
            server.Cancel();
            return 0;
        }
    }
    return -1;
}