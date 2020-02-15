#include "gtest/gtest.h"
#include <scheduler/httpserver/HttpServer.h>
#include <scheduler/SchedulerWorkerMP.h>
#include <communication/CommunicatorListener.h>

using namespace balancedbanana::scheduler;
using namespace balancedbanana::communication;

struct TestMP : SchedulerWorkerMP {

};

TEST(HttpServer, ApiTest)
{
    std::vector<std::shared_ptr<Worker>> worker;
    auto testmp = std::make_shared<TestMP>();
    auto listener = std::make_shared<CommunicatorListener>([testmp](){
        return testmp;
    });
    listener->listen(2435, [listener, &worker](std::shared_ptr<balancedbanana::communication::Communicator> com) {
        worker.emplace_back(std::make_shared<Worker>(com));
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
    }, [](int jobid) -> Job {
        return Job(jobid, QDateTime(QDate(2020, 1, 30), QTime(20, 10, 23, 0)), QDateTime(QDate(2020, 1, 30), QTime(22, 10, 23, 0)), QDateTime(QDate(2020, 1, 30), QTime(23, 10, 23, 0)), 100, 10, 0, "Steve", 2, 5, std::make_shared<balancedbanana::configfiles::JobConfig>(), std::make_shared<balancedbanana::database::JobStatus>(), std::make_shared<balancedbanana::database::job_result>()); 
    });
    auto com = std::make_shared<Communicator>("localhost", 2435, testmp);
    server.listen("localhost", 8234);
    com->detach();
}