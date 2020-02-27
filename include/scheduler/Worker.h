#pragma once
#include <communication/message/WorkerLoadResponseMessage.h>

#include <communication/Communicator.h>
#include <database/Specs.h>
#include "IUser.h"
#include "Observer.h"
#include <condition_variable>

namespace balancedbanana {
    namespace scheduler {
        class SchedulerWorkerMP;

        enum WorkerObservableEvent {
            DATA_CHANGE,
            HARDWARE_DETAIL_UPDATE
        };

        class Worker : public IUser, public Observable<WorkerObservableEvent> {
        public:
            void send(const communication::Message & msg);

            void getStatus();

            database::Specs getSpec();
            const communication::WorkerLoadResponseMessage& GetWorkerLoad();
            Worker(const std::shared_ptr<communication::Communicator>& comm);
        private:
            std::shared_ptr<communication::Communicator> comm;
            communication::WorkerLoadResponseMessage resp;
            std::condition_variable cnd;
        };
    }
}