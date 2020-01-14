#pragma once
#include <map>
#include <utility>
#include <string>

namespace balancedbanana {
    namespace communication {
        namespace authenticator {
            class Authenticator {
                std::pair<std::string, std::string> GeneratePrivatePublicKeyPair();

            };
        }
    }
}
