#pragma once

#include "IGateway.h"
#include "worker_details.h"
#include <cinttypes>
#include <vector>

namespace balancedbanana::database {
        class WorkerGateway : virtual public IGateway {
        public:
            uint64_t add(details details) override;
            bool remove(uint64_t id) override;
            worker_details getWorker(uint64_t id);
            std::vector<worker_details> getWorkers();

        private:
            static bool doesWorkerExist(uint64_t id);
        };
    }