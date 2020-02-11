#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {

		class PublicKeyAuthMessage : public Message {
            std::string username;

            std::string usernamesignature;
		public:
            PublicKeyAuthMessage(const std::string& username, const std::string& usernamesignature);

            explicit PublicKeyAuthMessage(const char *data, size_t &iterator, size_t size);

            void process(MessageProcessor &mp) const override;

            std::string serialize() const override;
		};
	}
}