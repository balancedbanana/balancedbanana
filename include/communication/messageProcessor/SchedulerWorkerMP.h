#pragma once
#include "MessageProcessor.h"

namespace balancedbanana {
    namespace communication {

        class SchedulerWorkerMP : public MessageProcessor {
            bool authenticated = false;
        public:
            virtual void processAuthResultMessage(const std::shared_ptr<AuthResultMessage>& msg) override;
            virtual void processWorkerAuthMessage(const std::shared_ptr<WorkerAuthMessage>& msg) override;
            virtual void processPublicKeyAuthMessage(const std::shared_ptr<PublicKeyAuthMessage>& msg) override;
            // virtual void processTaskMessage(const TaskMessage & msg);

        };
    }
}