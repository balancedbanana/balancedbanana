#pragma once
#include <communication/MessageProcessor.h>

namespace balancedbanana {
    namespace communication {
		
        class SchedulerClientMP : public MessageProcessor {
            bool authenticated = false;
        public:
#if 0
            SchedulerClientMP(Communicator *);
#endif

            void processClientAuthMessage(const ClientAuthMessage &msg) override;
            void processHardwareDetailMessage(const HardwareDetailMessage &msg) override;
            void processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg) override;
            void processTaskMessage(const TaskMessage &msg) override;
        };
    }
}