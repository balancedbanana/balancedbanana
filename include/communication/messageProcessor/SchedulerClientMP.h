#pragma once
#include "MessageProcessor.h"

namespace balancedbanana {
    namespace communication {
		
        class SchedulerClientMP : public MessageProcessor {
            bool authenticated = false;
        public:
            virtual void processAuthResultMessage(const std::shared_ptr<AuthResultMessage>& msg) override;
            virtual void processClientAuthMessage(const std::shared_ptr<ClientAuthMessage>& msg) override;
            virtual void processPublicKeyAuthMessage(const std::shared_ptr<PublicKeyAuthMessage>& msg) override;
            // virtual void processTaskMessage(const TaskMessage & msg);

        };
    }
}