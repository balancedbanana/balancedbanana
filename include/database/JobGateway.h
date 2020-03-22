#pragma once

#include "IGateway.h"
#include "job_details.h"
#include "job_result.h"
#include "JobStatus.h"
#include <configfiles/Priority.h>

#define FOUR_MB (4194304) // If RAM is under this amount, errors might occur in docker.
#define TIME_FORMAT "yyyy.MM.dd:hh.mm.ss.z" // The format of QDateTime objects in the database

namespace balancedbanana::database {
    class JobGateway : virtual public IGateway {

    public:
        explicit JobGateway(std::shared_ptr<QSqlDatabase> db);

        uint64_t add(job_details details);
        bool remove(uint64_t id);
        job_details getJob(uint64_t id);
        std::vector<job_details> getJobs();
        std::vector<job_details> getJobsWithWorkerId(uint64_t worker_id);
        bool startJob(uint64_t job_id, uint64_t worker_id, const Specs& specs, const QDateTime& start_time);
        bool finishJob(uint64_t job_id, const QDateTime& finish_time, const std::string& stdout, int8_t
        exit_code);
        std::vector<job_details> getJobsInInterval(const QDateTime &from, const QDateTime &to,
                JobStatus status);
        void updateJob(const job_details &job_details);
    };
}