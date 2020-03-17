#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {

		//Nachricht mit allen Hardware Angaben (CPU RAM OS)
		class HardwareDetailMessage : public Message {
            //Anzahl der CPU Kerne
            uint32_t coreCount;

            //Größe des verfügbaren Arbeitsspeichers in MiB
            uint64_t ramSize;

            //Irgendeine Information, die das Betriebssystem identifiziert
            std::string osIdentifier;
		public:
            HardwareDetailMessage(uint32_t coreCount, uint64_t ramSize, std::string  osIdentifier);

            HardwareDetailMessage(const char *data, size_t &iterator, size_t size);

            void process(MessageProcessor &mp) const override;

            std::string serialize() const override;

            uint32_t GetCoreCount() const;

            uint64_t GetRamSize() const;

            const std::string &GetOsIdentifier() const;
		};
	}
}