#pragma once
#include <communication/message/WorkerLoadResponseMessage.h>

#include <communication/Communicator.h>
#include <database/Specs.h>
#include "IUser.h"
#include "Observer.h"
#include "WorkerObserver.h"
#include <condition_variable>
#include <optional>

namespace balancedbanana {
    namespace scheduler {
        class SchedulerWorkerMP;

        enum class WorkerObservableEvent {
            DATA_CHANGE,
            STATUS_UPDATE,
            HARDWARE_DETAIL_UPDATE
        };

        class Worker : public IUser, public Observable<WorkerObservableEvent>/* , public Observable<WorkerFinishEvent> */, public Observable<WorkerTailEvent>, Observer<WorkerTailEvent>, public Observable<WorkerErrorEvent>, Observer<WorkerErrorEvent> {
        public:
            void send(const communication::Message & msg);

            bool isConnected();

            std::optional<database::Specs> getSpec();

            void setSpec(const std::optional<database::Specs>& spec);

            uint64_t getId();

            const communication::WorkerLoadResponseMessage& GetWorkerLoad();

            const std::string &getAddress();

            void setAddress(const std::string &adr);

            void setCommunicator(const std::shared_ptr<communication::Communicator>& com);

            Worker(uint64_t id, const std::string &name, const std::string &publickey, const
            std::optional<database::Specs> &specs);
        private:
            void OnUpdate(Observable<WorkerTailEvent> *obsable, WorkerTailEvent event) override;
            void OnUpdate(Observable<WorkerErrorEvent> *obsable, WorkerErrorEvent event) override;

            uint64_t id;
            std::optional<database::Specs> specs;
            bool connected;
            std::string address;
            std::shared_ptr<communication::Communicator> comm;
            communication::WorkerLoadResponseMessage resp;
            std::mutex mtx;
            std::condition_variable cnd;
        };
    }
}