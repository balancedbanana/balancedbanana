#pragma once
#include "SSLSocket.h"
#include "MessageProcessor.h"
#include "message/Message.h"
#include <memory>

namespace balancedbanana {
    namespace communication {
		class Communicator {
		private:
			std::shared_ptr<SSLSocket> socket;

			std::shared_ptr<MessageProcessor> processor;

		public:
			void Connect();

			void listen(const std::function<void(std::shared_ptr<Message>)>& callback);

			void send(const Message& message);

            static std::shared_ptr<Message> deserializeMessage(std::istream &stream);
		};
	}
}