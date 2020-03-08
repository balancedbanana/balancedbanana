#pragma once

#include "IGateway.h"
#include "worker_details.h"
#include <cinttypes>
#include <vector>

namespace balancedbanana::database {
        class WorkerGateway : virtual public IGateway {
        public:
            static uint64_t add(const worker_details& details);
            static bool remove(uint64_t id);
            static worker_details getWorker(uint64_t id);
            static std::vector<worker_details> getWorkers();
            static worker_details getWorkerByName(const std::string& name);
            static void updateWorker(const worker_details& worker);
        };
    }