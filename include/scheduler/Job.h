#pragma once
#include <QDateTime>
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
        public:
            Job(uint64_t id, const std::shared_ptr<configfiles::JobConfig> &config);
            Job(uint32_t id, const QDateTime &started_at, const QDateTime &scheduled_at, const QDateTime &finished_at,
                uint32_t allocated_ram, uint32_t allocated_cores, uint32_t allocated_disc_space,
                const std::string &command, uint64_t worker_id, uint64_t client_id,
                const std::shared_ptr<configfiles::JobConfig> &config,
                const std::shared_ptr<database::JobStatus> &status,
                const std::shared_ptr<database::job_result> &result);

            uint64_t id() const;
            const QDateTime &started_at() const;
            const QDateTime &scheduled_at() const;
            const QDateTime &finished_at() const;
            uint32_t allocated_ram() const;
            uint32_t allocated_cores() const;
            uint32_t allocated_disk_space() const;
            const std::string &command() const;
            uint64_t worker_id() const;
            uint64_t client_id() const;
            std::shared_ptr<configfiles::JobConfig> config() const;
            std::shared_ptr<database::JobStatus> status() const;
            std::shared_ptr<database::job_result> result() const;

            void set_id(uint32_t id);
            void set_started_at(const QDateTime &started_at);
            void set_scheduled_at(const QDateTime &scheduled_at);
            void set_finished_at(const QDateTime &finished_at);
            void set_allocated_ram(uint32_t allocated_ram);
            void set_allocated_cores(uint32_t allocated_cores);
            void set_allocated_disk_space(uint32_t allocated_disk_space);
            void set_command(const std::string &command);
            void set_worker_id(uint64_t worker_id);
            void set_client_id(uint64_t client_id);
            void set_config(std::shared_ptr<configfiles::JobConfig> &config);
            void set_status(std::shared_ptr<database::JobStatus> &status);
            void set_result(std::shared_ptr<database::job_result> &result);

        private:
            uint64_t id_;
            QDateTime started_at_;
            QDateTime scheduled_at_;
            QDateTime finished_at_;
            uint32_t allocated_ram_;
            uint32_t allocated_cores_;
            uint32_t allocated_disk_space_;
            std::string command_;
            uint64_t worker_id_;
            uint64_t client_id_;
            std::shared_ptr<configfiles::JobConfig> config_;
            std::shared_ptr<database::JobStatus> status_;
            std::shared_ptr<database::job_result> result_;
        };
    }
}