#pragma once
#include <communication/MessageProcessor.h>
#include <scheduler/Worker.h>
#include <functional>

namespace balancedbanana {
    namespace scheduler {

        class SchedulerWorkerMP : public communication::MessageProcessor {
            bool authenticated = false;
            std::function<void(const communication::WorkerLoadResponseMessage &msg)> onWorkerLoadResponseMessage;
            std::function<void(const std::shared_ptr<Worker> &worker)> addWorker;
            std::shared_ptr<communication::Communicator> com;
        public:
#if 0
            SchedulerWorkerMP(Communicator *);
#endif

            void processHardwareDetailMessage(const communication::HardwareDetailMessage &msg) override;
            void processPublicKeyAuthMessage(const communication::PublicKeyAuthMessage &msg) override;
            void processWorkerAuthMessage(const communication::WorkerAuthMessage &msg) override;
            void processWorkerLoadResponseMessage(const communication::WorkerLoadResponseMessage &msg) override;
            void OnWorkerLoadResponse(std::function<void(const communication::WorkerLoadResponseMessage &msg)>&& func);
            void setWorker(const std::shared_ptr<communication::Communicator>& com);
        };
    }
}