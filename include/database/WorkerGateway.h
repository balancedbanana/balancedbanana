#pragma once

#include "IGateway.h"
#include "worker_details.h"
#include <cinttypes>
#include <vector>

namespace balancedbanana::database {
        class WorkerGateway : virtual public IGateway {
        public:
            static uint64_t add(worker_details details);
            bool remove(uint64_t id) override;
            static worker_details getWorker(uint64_t id);
            static std::vector<worker_details> getWorkers();

        private:
            static bool doesWorkerExist(uint64_t id);
        };
    }