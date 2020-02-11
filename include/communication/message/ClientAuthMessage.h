#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {

		//Authenticates the client for its first connection to the scheduler
		class ClientAuthMessage : public Message {
            std::string username;
            std::string password;
            std::string publickey;
		public:
            ClientAuthMessage(const std::string &username, const std::string &password, const std::string &publickey);

		    ClientAuthMessage(const char *data, size_t &iterator, size_t size);

            void process(MessageProcessor &mp) const override;

            std::string serialize() const override;

			const std::string& GetUsername() const;

			const std::string& GetPassword() const;

			const std::string& GetPublickey() const;

		};

	}
}
