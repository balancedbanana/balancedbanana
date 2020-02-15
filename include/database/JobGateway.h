#pragma once

#include "IGateway.h"
#include "job_details.h"
#include "job_result.h"

namespace balancedbanana::database {
    class JobGateway : virtual public IGateway {
    public:
        static uint64_t add(job_details details);
        static bool remove(uint64_t id);
        static job_details getJob(uint64_t id);
        static std::vector<job_details> getJobs();
        static bool startJob(uint64_t job_id, uint64_t worker_id, Specs specs);
        static bool finishJob(uint64_t job_id, const QDateTime& finish_tim, const std::string& stdout, const int8_t
        exit_code);
        static job_result getJobResult(uint64_t job_id);
    };
}