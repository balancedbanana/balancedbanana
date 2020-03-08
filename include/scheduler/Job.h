#pragma once
#include <QDateTime>
#include <configfiles/JobConfig.h>
#include <database/JobStatus.h>
#include <database/job_result.h>
#include <memory>
#include <cstdint>
#include <string>
#include <chrono>
#include "Observer.h"
#include "User.h"

namespace balancedbanana {
    namespace scheduler {

        enum class JobObservableEvent {
            DATA_CHANGE
        };

        class Job : public Observable<JobObservableEvent> {
        public:
            Job(uint64_t id, std::shared_ptr<configfiles::JobConfig> config);
            Job(uint32_t id, QDateTime started_at, QDateTime scheduled_at, QDateTime finished_at,
                uint32_t allocated_ram, uint32_t allocated_cores, uint32_t allocated_disc_space,
                std::string command, uint64_t worker_id, std::shared_ptr<User> user,
                std::shared_ptr<configfiles::JobConfig> config,
                database::JobStatus status,
                std::shared_ptr<database::job_result> result);

            uint64_t getId() const;
            const QDateTime &getStarted_at() const;
            const QDateTime &getScheduled_at() const;
            const QDateTime &getFinished_at() const;
            uint32_t getAllocated_ram() const;
            uint32_t getAllocated_cores() const;
            uint32_t getAllocated_disk_space() const;
            const std::string &getCommand() const;
            uint64_t getWorker_id() const;
            std::shared_ptr<User> getUser() const;
            std::shared_ptr<configfiles::JobConfig> getConfig() const;
            database::JobStatus getStatus() const;
            std::shared_ptr<database::job_result> getResult() const;

            void setId(uint32_t id);
            void setStarted_at(const QDateTime &started_at);
            void setScheduled_at(const QDateTime &scheduled_at);
            void setFinished_at(const QDateTime &finished_at);
            void setAllocated_ram(uint32_t allocated_ram);
            void setAllocated_cores(uint32_t allocated_cores);
            void setAllocated_disk_space(uint32_t allocated_disk_space);
            void setCommand(const std::string &command);
            void setWorker_id(uint64_t worker_id);
            void setUser(const std::shared_ptr<User> &user);
            void setConfig(const std::shared_ptr<configfiles::JobConfig> &config);
            void setStatus(database::JobStatus status);
            void setResult(const std::shared_ptr<database::job_result> &result);

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
            std::shared_ptr<User> user_;
            std::shared_ptr<configfiles::JobConfig> config_;
            database::JobStatus status_;
            std::shared_ptr<database::job_result> result_;
        };
    }
}