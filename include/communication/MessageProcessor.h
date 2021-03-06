#pragma once
#include <memory>

namespace balancedbanana {
    namespace communication {

        class Communicator;
		class AuthResultMessage;
		class ClientAuthMessage;
		class HardwareDetailMessage;
		class TaskResponseMessage;
		class Message;
		class PublicKeyAuthMessage;
		class TaskMessage;
		class WorkerAuthMessage;
		class WorkerLoadRequestMessage;
		class WorkerLoadResponseMessage;
		class RespondToClientMessage;

        class MessageProcessor {
        private:
#if 0
            std::shared_ptr<Communicator> communicator_;
#endif

            virtual void handleInvalidMessage(const Message &msg);

        public:
#if 0
            explicit MessageProcessor(std::shared_ptr<Communicator> communicator = nullptr);
#endif

            void process(const std::shared_ptr<Message> &msg);
#if 0
            std::shared_ptr<Communicator> communicator();
#endif

            virtual void onDisconnect();
            virtual void processAuthResultMessage(const AuthResultMessage &msg);
            virtual void processClientAuthMessage(const ClientAuthMessage &msg);
            virtual void processHardwareDetailMessage(const HardwareDetailMessage &msg);
            virtual void processTaskResponseMessage(const TaskResponseMessage &msg);
            virtual void processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg);
            virtual void processTaskMessage(const TaskMessage &msg);
            virtual void processWorkerAuthMessage(const WorkerAuthMessage &msg);
            virtual void processWorkerLoadRequestMessage(const WorkerLoadRequestMessage &msg);
            virtual void processWorkerLoadResponseMessage(const WorkerLoadResponseMessage &msg);
            virtual void processRespondToClientMessage(const RespondToClientMessage &msg);
        };
	}
}

