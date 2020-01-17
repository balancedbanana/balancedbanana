#pragma once
#include <configfiles/JobConfig.h>
#include <database/JobStatus.h>
#include <database/job_result.h>
#include <memory>
#include <cstdint>
#include <string>
#include <chrono>

namespace balancedbanana {
    namespace scheduler {

        class Job {
        private:
            uint64_t id;

            std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> started_at;

            std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> scheduled_at;

            std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> finished_at;

            uint64_t allocated_ram;

            uint32_t allocated_cores;

            uint64_t allocated_disk_space;

            std::string command;

            uint64_t worker_id;

            uint64_t user_id;

            std::shared_ptr<configfiles::JobConfig> config;

            std::shared_ptr<database::JobStatus> status;

            std::shared_ptr<database::job_result> result;
        };
    }
}