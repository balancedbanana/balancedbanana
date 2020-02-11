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

            void process(MessageProcessor &mp) const override;

            std::string serialize() const override;
		};
	}
}