#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {

		//Authenifiziert einen Arbeiter für die erste Verbinung  ( folgend PublicKeyAuthMessage on verwenden)
		class ClientAuthMessage : public Message {
            std::string username;
            std::string password;
            std::string publickey;
		public:
            ClientAuthMessage(const std::string& username, const std::string& password, const std::string& pubkey);

		    explicit ClientAuthMessage(std::istream &stream);

			virtual void process(const std::shared_ptr<MessageProcessor>& mp);

            virtual void serialize(std::ostream &stream) override;
		};

	}
}
