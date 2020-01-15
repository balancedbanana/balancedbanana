#pragma once
#include <map>
#include <utility>
#include <string>
#include <communication/Communicator.h>

namespace balancedbanana {
    namespace communication {
        namespace authenticator {
            class Authenticator {
                std::pair<std::string, std::string> GeneratePrivatePublicKeyPair();
                std::string GenerateSignature(std::string name, std::string privkey);

			public:
				Authenticator(const Communicator& comm);

				void authenticate(const std::string& username, const std::string& password);

				void authenticate(const std::string& username);

				//Nur für worker
				void authenticate();
            };
        }
    }
}