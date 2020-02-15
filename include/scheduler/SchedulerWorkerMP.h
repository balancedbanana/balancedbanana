#pragma once
#include <communication/MessageProcessor.h>
#include <functional>

namespace balancedbanana {
    namespace scheduler {

        class SchedulerWorkerMP : public communication::MessageProcessor {
            bool authenticated = false;
            std::function<void(const communication::WorkerLoadResponseMessage &msg)> onWorkerLoadResponseMessage;
        public:
#if 0
            SchedulerWorkerMP(Communicator *);
#endif

            void processHardwareDetailMessage(const communication::HardwareDetailMessage &msg) override;
            void processPublicKeyAuthMessage(const communication::PublicKeyAuthMessage &msg) override;
            void processSnapshotMessage(const communication::SnapshotMessage &msg) override;
            void processTaskMessage(const communication::TaskMessage &msg) override;
            void processWorkerAuthMessage(const communication::WorkerAuthMessage &msg) override;
            void processWorkerLoadResponseMessage(const communication::WorkerLoadResponseMessage &msg) override;
            void OnWorkerLoadResponse(std::function<void(const communication::WorkerLoadResponseMessage &msg)>&& func);
        };
    }
}