#pragma once

#include "Message.h"

namespace balancedbanana {
    namespace communication {

		class WorkerAuthMessage : public Message {
		public:
			virtual void process(const std::shared_ptr<MessageProcessor>& mp);


		private:
			std::string workername;


		public:
			WorkerAuthMessage(const std::string& workername, const std::string& pubkey);


		private:
			std::string publickey;

		};
	}
}