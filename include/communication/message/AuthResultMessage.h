#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {

		class AuthResultMessage : public Message {
			uint32_t status;

		public:
		    AuthResultMessage(uint32_t status);

		    explicit AuthResultMessage(std::istream &stream);

			//Gibt den Authentfication status zurück 0 falls erfolgreich sonst ungleich 0
			uint32_t getStatus();

			virtual void process(const std::shared_ptr<MessageProcessor>& mp);

			virtual void serialize(std::ostream &stream) override;
		};

	}
}