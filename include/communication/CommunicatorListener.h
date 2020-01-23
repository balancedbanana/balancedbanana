#pragma once
#include <Net/TLSSocketListener.h>
#include "messageProcessor/MessageProcessor.h"
#include "message/Message.h"
#include <memory>

namespace balancedbanana {
    namespace communication {
		class CommunicatorListener {
		private:
			std::shared_ptr<Net::TLSSocketListener> listener;

			std::shared_ptr<MessageProcessor> processor;

			std::shared_ptr<std::thread> listenthread;

		public:
			CommunicatorListener(const std::shared_ptr<MessageProcessor>& processor);
			void listen(const std::function<void(std::shared_ptr<Message>)>& callback);
		};
	}
}