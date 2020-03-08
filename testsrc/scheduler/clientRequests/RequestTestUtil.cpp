#include "RequestTestUtil.h"

using namespace balancedbanana::scheduler;
using namespace balancedbanana::database;
using namespace balancedbanana::configfiles;

std::shared_ptr<Job> dbGetJob(uint64_t jobID)
{
}

void dbUpdateJobStatus(uint64_t jobID, JobStatus newStatus)
{
}

uint64_t dbAddJob(uint64_t userID, const std::shared_ptr<JobConfig>& config, const std::string& command)
{
}