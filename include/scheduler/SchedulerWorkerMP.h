#pragma once
#include <communication/MessageProcessor.h>

namespace balancedbanana {
    namespace communication {

        class SchedulerWorkerMP : public MessageProcessor {
            bool authenticated = false;
        public:
#if 0
            SchedulerWorkerMP(Communicator *);
#endif

            void processAuthResultMessage(const AuthResultMessage &msg) override;
            void processClientAuthMessage(const ClientAuthMessage &msg) override;
            void processHardwareDetailMessage(const HardwareDetailMessage &msg) override;
            void processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg) override;
            void processSnapshotMessage(const SnapshotMessage &msg) override;
            void processTaskMessage(const TaskMessage &msg) override;
            void processWorkerAuthMessage(const WorkerAuthMessage &msg) override;
        };
    }
}