#pragma once
#include <Net/TLSSocketListener.h>
#include "messageProcessor/MessageProcessor.h"
#include "message/Message.h"
#include "Communicator.h"
#include <memory>

namespace balancedbanana {
    namespace communication {
		class CommunicatorListener {
		private:
			std::shared_ptr<Net::TLSSocketListener> listener;

			std::function<std::shared_ptr<MessageProcessor>()> processorfactory;

			std::shared_ptr<std::thread> listenthread;

		public:
			CommunicatorListener(std::function<std::shared_ptr<MessageProcessor>()> processorfactory);
			void listen(const std::function<void(std::shared_ptr<Communicator>)>& callback);
		};
	}
}