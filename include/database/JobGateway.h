#pragma once

#include "IGateway.h"
#include "job_details.h"
#include "job_result.h"

namespace balancedbanana::database {
    class JobGateway : virtual public IGateway {
    public:
        uint64_t add(job_details details);
        bool remove(uint64_t id) override;
        job_details getJob(uint64_t id);
        std::vector<job_details> getJobs();
        bool startJob(uint64_t job_id, uint64_t worker_id, Specs specs);
        bool finishJob(uint64_t job_id, const QDateTime& finish_tim, const std::string& stdout, const int8_t exit_code);
        job_result getJobResult(uint64_t job_id);

    private:
        static bool doesJobExist(uint64_t id);
    };
}