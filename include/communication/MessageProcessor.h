#pragma once
#include <memory>

namespace balancedbanana {
    namespace communication {

        class Communicator;
		class AuthResultMessage;
		class ClientAuthMessage;
		class HardwareDetailMessage;
		class Message;
		class PublicKeyAuthMessage;
		class SnapshotMessage;
		class TaskMessage;
		class WorkerAuthMessage;

        class MessageProcessor : public std::enable_shared_from_this<MessageProcessor> {
        private:
            std::shared_ptr<Communicator> communicator_;

            virtual void handleInvalidMessage(const Message &msg);

        public:
            explicit MessageProcessor(std::shared_ptr<Communicator> communicator);

            void process(const std::shared_ptr<Message> &msg);
            std::shared_ptr<Communicator> communicator();

            virtual void processAuthResultMessage(const AuthResultMessage &msg);
            virtual void processClientAuthMessage(const ClientAuthMessage &msg);
            virtual void processHardwareDetailMessage(const HardwareDetailMessage &msg);
            virtual void processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg);
            virtual void processSnapshotMessage(const SnapshotMessage &msg);
            virtual void processTaskMessage(const TaskMessage &msg);
            virtual void processWorkerAuthMessage(const WorkerAuthMessage &msg);

        };
	}
}

