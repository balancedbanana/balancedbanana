#pragma once

#include "Message.h"

namespace balancedbanana {
    namespace communication {

		class PublicKeyAuthMessage : public Message {
		public:
			virtual void process(const std::shared_ptr<MessageProcessor>& mp);


		private:
			std::string username;

			std::string usernamesignature;


		public:
			PublicKeyAuthMessage(const std::string& username, const std::string& usernamesignature);

		};
	}
}