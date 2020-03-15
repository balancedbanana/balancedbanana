#include "TestClientRequestsUtils.h"


std::shared_ptr<Job> dbGetJob(uint64_t jobID)
{
    struct job_result job_result{"result", 0};

    return std::make_shared<Job>(jobID, QDateTime::currentDateTime(), QDateTime::currentDateTime(), QDateTime::currentDateTime(),
                                 1024, 4, 4096,
                                 "echo \"Hello World!\"", 0, std::make_shared<User>(0, "Username", "Publickey"),
                                 std::make_shared<JobConfig>(), JobStatus::scheduled,
                                 std::make_shared<struct job_result>(job_result));
}

std::shared_ptr<Worker> dbGetWorker(uint64_t workerID)
{
    struct Specs specs{"GNU/Linux", 8192, 8};

    return std::make_shared<Worker>(workerID, "Workername", "Publickey", specs);
}

std::shared_ptr<Job> dbAddJob(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)
{
    struct job_result job_result = {"result", 0};

    return std::make_shared<Job>(0, scheduleTime, scheduleTime, scheduleTime,
                                 1024, 4, 4096,
                                 jobCommand, 0, std::make_shared<User>(userID, "Username", "Publickey"),
                                 config, JobStatus::scheduled,
                                 std::make_shared<struct job_result>(job_result));
}

bool queueRemoveJob(uint64_t jobID)
{
    return true;
}

uint64_t queueGetPosition(uint64_t jobID)
{
    return 1;
}
