#pragma once

#include "IGateway.h"
#include "job_details.h"
#include "job_result.h"
#include "JobStatus.h"

#define FOUR_MB (4194304) // If RAM is under this amount, errors might occur in docker.

namespace balancedbanana::database {
    class JobGateway : virtual public IGateway {
    public:
        static uint64_t add(job_details details);
        static bool remove(uint64_t id);
        static job_details getJob(uint64_t id);
        static std::vector<job_details> getJobs();
        static std::vector<job_details> getJobsWithWorkerId(uint64_t worker_id);
        static bool startJob(uint64_t job_id, uint64_t worker_id, Specs specs, const QDateTime& start_time);
        static bool finishJob(uint64_t job_id, const QDateTime& finish_time, const std::string& stdout, int8_t
        exit_code);
        static job_result getJobResult(uint64_t job_id);
        static std::vector<job_details> getJobsInInterval(const QDateTime &from, const QDateTime &to,
                JobStatus status);
        static void updateJob(const job_details &job_details);
    };
}