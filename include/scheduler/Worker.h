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

            bool isConnected();

            database::Specs getSpec();

            uint64_t getId();

            const communication::WorkerLoadResponseMessage& GetWorkerLoad();

            const std::string &getAddress();

            void setAddress(const std::string &adr);

            void setCommunicator(const std::shared_ptr<communication::Communicator>& com);

            Worker(uint64_t id, const std::string &name, const std::string &publickey, const database::Specs &specs);
        private:
            uint64_t id;
            database::Specs specs;
            bool connected;
            std::string address;
            std::shared_ptr<communication::Communicator> comm;
            communication::WorkerLoadResponseMessage resp;
            std::mutex mtx;
            std::condition_variable cnd;
        };
    }
}