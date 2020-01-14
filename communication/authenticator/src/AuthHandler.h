#pragma once
#include <string>
#include <memory>

namespace balancedbanana {
    namespace communication {
        namespace authenticator {
            class IUser {
                std::string name_;
                std::string pubkey_;
            public:
                std::string name() { return name_; };
                std::string pubkey() { return pubkey_; };
            };
            class AuthHandler {
                void authenticate(std::shared_ptr<IUser> user, std::string signature);
            };
        }
    }
}