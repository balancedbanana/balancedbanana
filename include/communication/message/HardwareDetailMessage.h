#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {

		//Nachricht mit allen Hardware Angaben (CPU RAM OS)
		class HardwareDetailMessage : public Message {
		public:
			void process(const std::shared_ptr<MessageProcessor> & msgProcessor);


		private:
			//Anzahl der CPU Kerne
			int coreCount;

			//Größe des verfügbaren Arbeitsspeichers
			int ramSize;

			//Irgendeine Information, die das Betriebssystem identifiziert
			std::string osIdentifier;


		public:
			HardwareDetailMessage(int coreCount, int ramSize, const std::string& osIdentifier);

		};
	}
}