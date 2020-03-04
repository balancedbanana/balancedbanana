#pragma once
#include <communication/message/WorkerLoadResponseMessage.h>

#include <communication/Communicator.h>
#include <database/Specs.h>
#include "IUser.h"
#include <condition_variable>

namespace balancedbanana {
    namespace scheduler {
        class SchedulerWorkerMP;
        class Worker : public IUser {
        public:
            void send(const communication::Message & msg);

            void getStatus();

            database::Specs getSpec();
            const communication::WorkerLoadResponseMessage& GetWorkerLoad();
            Worker(const std::shared_ptr<communication::Communicator>& comm);

            static Worker getWorker(uint64_t workerID) { return Worker(nullptr); }

        private:
            std::shared_ptr<communication::Communicator> comm;
            communication::WorkerLoadResponseMessage resp;
            std::condition_variable cnd;
        };
    }
}