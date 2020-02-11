#pragma once
#include "MessageProcessor.h"
#include <Net/Socket.h>
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

			//TODO shouldnt it be a string& ?
			Communicator(const std::string address, short port, const std::shared_ptr<MessageProcessor>& processor);

			void send(const Message& message);
		};
	}
}