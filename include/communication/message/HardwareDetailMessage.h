#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {

		//Nachricht mit allen Hardware Angaben (CPU RAM OS)
		class HardwareDetailMessage : public Message {
            //Anzahl der CPU Kerne
            uint32_t coreCount;

            //Größe des verfügbaren Arbeitsspeichers
            uint32_t ramSize;

            //Irgendeine Information, die das Betriebssystem identifiziert
            std::string osIdentifier;
		public:
            HardwareDetailMessage(uint32_t coreCount, uint32_t ramSize, const std::string& osIdentifier);

            explicit HardwareDetailMessage(std::istream &stream);

			void process(const std::shared_ptr<MessageProcessor> & msgProcessor);

            virtual void serialize(std::ostream &stream) override;
		};
	}
}