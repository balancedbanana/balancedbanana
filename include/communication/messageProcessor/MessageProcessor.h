#pragma once
#include <memory>

namespace balancedbanana {
    namespace communication {

		class AuthResultMessage;
		class ClientAuthMessage;
		class HardwareDetailMessage;
		class Message;
		class PublicKeyAuthMessage;
		class SnapshotMessage;
		class TaskMessage;
		class WorkerAuthMessage;

        class MessageProcessor {
        private:
            virtual void handleInvalidMessage(const std::shared_ptr<Message> &msg);

        public:
            void process(const std::shared_ptr<Message> &msg);

            virtual void processAuthResultMessage(const std::shared_ptr<AuthResultMessage> &msg);
            virtual void processClientAuthMessage(const std::shared_ptr<ClientAuthMessage> &msg);
            virtual void processHardwareDetailMessage(const std::shared_ptr<HardwareDetailMessage> &msg);
            virtual void processPublicKeyAuthMessage(const std::shared_ptr<PublicKeyAuthMessage> &msg);
            virtual void processSnapshotMessage(const std::shared_ptr<SnapshotMessage> &msg);
            virtual void processTaskMessage(const std::shared_ptr<TaskMessage> &msg);
            virtual void processWorkerAuthMessage(const std::shared_ptr<WorkerAuthMessage> &msg);

        };
	}
}

