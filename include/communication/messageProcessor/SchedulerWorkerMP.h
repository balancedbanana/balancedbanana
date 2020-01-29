#pragma once
#include <communication/messageProcessor/MessageProcessor.h>

namespace balancedbanana {
    namespace communication {

        class SchedulerWorkerMP : public MessageProcessor {
            bool authenticated = false;
        public:
            SchedulerWorkerMP(Communicator *);

            void processAuthResultMessage(const std::shared_ptr<AuthResultMessage> &msg) override;
            void processClientAuthMessage(const std::shared_ptr<ClientAuthMessage> &msg) override;
            void processHardwareDetailMessage(const std::shared_ptr<HardwareDetailMessage> &msg) override;
            void processPublicKeyAuthMessage(const std::shared_ptr<PublicKeyAuthMessage> &msg) override;
            void processSnapshotMessage(const std::shared_ptr<SnapshotMessage> &msg) override;
            void processTaskMessage(const std::shared_ptr<TaskMessage> &msg) override;
            void processWorkerAuthMessage(const std::shared_ptr<WorkerAuthMessage> &msg) override;
        };
    }
}