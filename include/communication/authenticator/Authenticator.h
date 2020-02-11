#pragma once
#include <map>
#include <utility>
#include <string>
#include <communication/Communicator.h>

namespace balancedbanana {
    namespace communication {
        namespace authenticator {
            class Authenticator {
                std::shared_ptr<balancedbanana::communication::Communicator> comm;
			public:
                static std::pair<std::string, std::string> GeneratePrivatePublicKeyPair();
                static std::string GenerateSignature(std::string name, std::string privkey);

				Authenticator(const std::shared_ptr<balancedbanana::communication::Communicator> &comm);

                // authenticates clients with password and returns the private key
				std::string authenticate(const std::string& username, const std::string& password);
                
                // authenticates client / worker agaist the server
				void publickeyauthenticate(const std::string& username, const std::string& pubkey);

				// worker auth returns worker name and private key
				std::pair<std::string, std::string> authenticate();
            };
        }
    }
}