#include <communication/message/HardwareDetailMessage.h>

#include <utility>

using namespace balancedbanana::communication;
using namespace serialization;

HardwareDetailMessage::HardwareDetailMessage(uint32_t coreCount, uint32_t ramSize, std::string  osIdentifier) :
Message(HARDWARE_DETAIL), coreCount(coreCount), ramSize(ramSize), osIdentifier(std::move(osIdentifier)) {
}

HardwareDetailMessage::HardwareDetailMessage(const char *data, size_t &iterator, size_t size) :
Message(HARDWARE_DETAIL), coreCount(0), ramSize(0), osIdentifier("") {
    coreCount = extract<uint32_t>(data, iterator, size);
    ramSize = extract<uint32_t>(data, iterator, size);
    osIdentifier = extractString(data, iterator, size);
}

void HardwareDetailMessage::process(MessageProcessor &mp) const {
    mp.processHardwareDetailMessage(*this);
}

std::string HardwareDetailMessage::serialize() const {
    std::stringstream s;
    s << Message::serialize();
    insert<uint32_t>(s, coreCount);
    insert<uint32_t>(s, ramSize);
    insertString(s, osIdentifier);
    return s.str();
}

uint32_t HardwareDetailMessage::GetCoreCount() const {
    return coreCount;
}

uint32_t HardwareDetailMessage::GetRamSize() const {
    return ramSize;
}

const std::string &HardwareDetailMessage::GetOsIdentifier() const {
    return osIdentifier;
}