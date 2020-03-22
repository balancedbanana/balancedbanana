#pragma once

#include "IGateway.h"
#include "worker_details.h"
#include <cinttypes>
#include <vector>

namespace balancedbanana::database {
        class WorkerGateway : virtual public IGateway {

        public:
            explicit WorkerGateway(std::shared_ptr<QSqlDatabase> db);

            uint64_t add(const worker_details& details);
            bool remove(uint64_t id);
            worker_details getWorker(uint64_t id);
            std::vector<worker_details> getWorkers();
            worker_details getWorkerByName(const std::string& name);
            void updateWorker(const worker_details& worker);
        };
    }