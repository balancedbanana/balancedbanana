#pragma once
#include <memory>
#include <cstdint>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>
#include <database/JobStatus.h>

std::shared_ptr<balancedbanana::scheduler::Job> dbGetJob(uint64_t jobID);

void dbUpdateJobStatus(uint64_t jobID, balancedbanana::database::JobStatus newStatus);

uint64_t dbAddJob(uint64_t userID, const std::shared_ptr<balancedbanana::configfiles::JobConfig>& config);