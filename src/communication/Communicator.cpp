#include <communication/Communicator.h>

using namespace balancedbanana::communication;

void Communicator::Connect() {

}

std::shared_ptr<Message> Communicator::deserializeMessage(std::istream &stream) {
    MessageType type;
    stream.readsome((char *) &type, sizeof(uint32_t));
    switch(type) {
        case AUTH_RESULT:

            break;
        case CLIENT_AUTH:

            break;
        case HARDWARE_DETAIL:

            break;
        case PUBLIC_KEY_AUTH:

            break;
        case SNAPSHOT:

            break;
        case TASK:

            break;
        case WORKER_AUTH:

            break;
        default:

            break;
    }
    return nullptr;
}