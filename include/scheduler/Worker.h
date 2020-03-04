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

        enum class WorkerObservableEvent {
            DATA_CHANGE,
            STATUS_UPDATE,
            HARDWARE_DETAIL_UPDATE
        };

        class Worker : public IUser, public Observable<WorkerObservableEvent> {
        public:
            void send(const communication::Message & msg);

            void requestStatus(Observer<WorkerObservableEvent> &obs);

            database::Specs getSpec();
            void setSpecs(const database::Specs &specs);

            uint64_t getId();

            const communication::WorkerLoadResponseMessage& GetWorkerLoad();
            Worker(const std::shared_ptr<communication::Communicator>& comm);
        private:
            uint64_t id;
            database::Specs specs;



            std::shared_ptr<communication::Communicator> comm;
            communication::WorkerLoadResponseMessage resp;
            std::condition_variable cnd;
        };
    }
}