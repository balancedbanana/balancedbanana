#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {

		class PublicKeyAuthMessage : public Message {
            std::string username;

            std::string usernamesignature;
		public:
            PublicKeyAuthMessage(const std::string& username, const std::string& usernamesignature);

            explicit PublicKeyAuthMessage(std::istream &stream);

			virtual void process(const std::shared_ptr<MessageProcessor>& mp);

            virtual void serialize(std::ostream &stream) override;
		};
	}
}