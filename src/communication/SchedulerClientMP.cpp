#include <communication/messageProcessor/SchedulerClientMP.h>
#include <communication/message/ClientAuthMessage.h>
#include <communication/authenticator/AuthHandler.h>

using namespace balancedbanana::communication;

void SchedulerClientMP::processClientAuthMessage(const std::shared_ptr<ClientAuthMessage>& msg) {
    auto user = std::make_shared<balancedbanana::scheduler::IUser>(msg->GetUsername(), msg->GetPublickey());
    authenticator::AuthHandler::GetDefault()->authenticate(user, msg->GetPassword());
}

void SchedulerClientMP::processPublicKeyAuthMessage(const std::shared_ptr<PublicKeyAuthMessage>& msg) {

}