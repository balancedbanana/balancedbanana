#pragma once

#include <inttypes.h>
#include <memory>
#include <scheduler/Job.h>
#include <scheduler/Worker.h>
#include <scheduler/User.h>
#include <communication/Task.h>
#include <configfiles/JobConfig.h>
#include <QDateTime>
#include <database/job_result.h>
#include <database/Specs.h>
#include <database/JobStatus.h>

using balancedbanana::communication::Task;
using balancedbanana::configfiles::JobConfig;
using balancedbanana::database::job_result;
using balancedbanana::database::Specs;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::scheduler::User;
using balancedbanana::scheduler::Worker;

constexpr uint64_t userID = 0;

std::shared_ptr<Job> dbGetJob(uint64_t jobID);

std::shared_ptr<Worker> dbGetWorker(uint64_t workerID);

std::shared_ptr<Job> dbAddJob(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand);

uint64_t queueGetPosition(uint64_t jobID);