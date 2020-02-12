#pragma once
#include <communication/MessageProcessor.h>

namespace balancedbanana {
    namespace communication {
		
        class SchedulerClientMP : public MessageProcessor {
            bool authenticated = false;
        public:
            SchedulerClientMP(Communicator *);

            void processClientAuthMessage(const ClientAuthMessage &msg) override;
            void processHardwareDetailMessage(const HardwareDetailMessage &msg) override;
            void processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg) override;
            void processSnapshotMessage(const SnapshotMessage &msg) override;
            void processTaskMessage(const TaskMessage &msg) override;
        };
    }
}