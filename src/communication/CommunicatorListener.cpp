#include <communication/CommunicatorListener.h>
#include <communication/Communicator.h>
#include <communication/authenticator/Authenticator.h>

using namespace balancedbanana::communication;

CommunicatorListener::CommunicatorListener(const std::shared_ptr<MessageProcessor>& processor) {
    this->processor = processor;
}

void CommunicatorListener::listen(const std::function<void(std::shared_ptr<Message>)>& callback) {
    listener = std::make_shared<Net::TLSSocketListener>();
    listener->SetConnectionHandler([this](std::shared_ptr<Net::Socket> socket) {
        auto com = std::make_shared<Communicator>(socket, processor);
    });
    auto address = std::make_shared<sockaddr_in6>();
	memset(address.get(), 0, sizeof(sockaddr_in6));
	address->sin6_family = AF_INET6;
	address->sin6_port = htons(8443);
	address->sin6_addr = in6addr_any;
    // auto p = balancedbanana::communication::authenticator::Authenticator::GeneratePrivatePublicKeyPair();
    // listener->UseCertificate((uint8_t*)p.second.data(), (int)p.second.length(), Net::SSLFileType::PEM);
    // listener->UsePrivateKey((uint8_t*)p.first.data(), (int)p.first.length(), Net::SSLFileType::PEM);
    listener->UseCertificate("server.cert", Net::SSLFileType::PEM);
    listener->UsePrivateKey("server.key", Net::SSLFileType::PEM);
    listenthread = listener->Listen(std::shared_ptr<sockaddr>(address, (sockaddr*)address.get()), sizeof(sockaddr_in6));
    listenthread->join();
}