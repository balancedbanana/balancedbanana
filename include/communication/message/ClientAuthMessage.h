#ifndef CLIENTAUTHMESSAGE_H
#define CLIENTAUTHMESSAGE_H

#include "Message.h"

namespace balancedbanana {
    namespace communication {

		//Authenticates the client for its first connection to the scheduler
		class ClientAuthMessage : public Message {
			std::string username;
			std::string password;
			std::string publickey;
		public:
			ClientAuthMessage(const std::string& username, const std::string& password, const std::string& pubkey);
			virtual void process(const std::shared_ptr<MessageProcessor>& mp) override;
			virtual std::string serialize();
			const std::string& GetUsername();
			const std::string& GetPassword();
			const std::string& GetPublickey();
		};
	}
}

#endif // !CLIENTAUTHMESSAGE_H
