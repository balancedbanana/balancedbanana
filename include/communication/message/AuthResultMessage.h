#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {

		class AuthResultMessage : public Message {
			uint32_t status;

		public:
		    explicit AuthResultMessage(uint32_t status);

		    explicit AuthResultMessage(const char *data, size_t &iterator, size_t length);

			//Gibt den Authentfication status zurück 0 falls erfolgreich sonst ungleich 0
			uint32_t getStatus() const;

			void process(MessageProcessor &mp) const override;

			std::string serialize() const override;
		};

	}
}