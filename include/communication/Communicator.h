#pragma once
#include <Net/Socket.h>
#include "messageProcessor/MessageProcessor.h"
#include "message/Message.h"
#include <memory>

namespace balancedbanana {
    namespace communication {
		class Communicator {
		private:
			std::shared_ptr<Net::Socket> socket;

			std::shared_ptr<MessageProcessor> processor;

		public:
			Communicator(const std::shared_ptr<Net::Socket> &socket, const std::shared_ptr<MessageProcessor>& processor);

			Communicator(const std::string address, short port, const std::shared_ptr<MessageProcessor>& processor);

			void send(const Message& message);
			

		};
	}
}