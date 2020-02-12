#pragma once
#include "MessageProcessor.h"
#include <Net/Socket.h>
#include "message/Message.h"
#include <memory>
#include <thread>
#include <atomic>

namespace balancedbanana {
    namespace communication {
		class Communicator {
		private:
			std::shared_ptr<Net::Socket> socket;
			std::thread msghandlerthread;
			std::shared_ptr<std::atomic_bool> keeprunning;
			static void msghandler(std::shared_ptr<Net::Socket>, std::shared_ptr<MessageProcessor> processor, std::shared_ptr<std::atomic_bool> keeprunning);
		public:
			Communicator(const std::shared_ptr<Net::Socket> &socket, const std::shared_ptr<MessageProcessor>& processor);

			//TODO shouldnt it be a string& ?
			Communicator(const std::string address, short port, const std::shared_ptr<MessageProcessor>& processor);

			~Communicator();

			void send(const Message& message);
			
			void detach();
		};
	}
}