#include <communication/message/HardwareDetailMessage.h>

#include <utility>

using namespace balancedbanana::communication;
using namespace serialization;

HardwareDetailMessage::HardwareDetailMessage(uint32_t coreCount, uint32_t ramSize, std::string  osIdentifier) :
Message(HARDWARE_DETAIL), coreCount(coreCount), ramSize(ramSize), osIdentifier(std::move(osIdentifier)) {
}

HardwareDetailMessage::HardwareDetailMessage(const char *data, size_t &iterator, size_t size) :
Message(HARDWARE_DETAIL), coreCount(0), ramSize(0), osIdentifier("") {
    coreCount = extractUInt32(data, iterator, size);
    ramSize = extractUInt32(data, iterator, size);
    osIdentifier = extractString(data, iterator, size);
}

void HardwareDetailMessage::process(MessageProcessor &mp) const {
    mp.processHardwareDetailMessage(*this);
}

std::string HardwareDetailMessage::serialize() const {
    std::stringstream s;
    s << Message::serialize();
    insert(s, coreCount);
    insert(s, ramSize);
    insert(s, osIdentifier);
}