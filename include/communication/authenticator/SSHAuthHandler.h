#pragma once
#include "AuthHandler.h"

namespace balancedbanana {
    namespace communication {
        namespace authenticator {
            class SSHAuthHandler : public AuthHandler {
            public:
                virtual void authenticate(const std::shared_ptr<scheduler::IUser>& user, const std::string& password) override;
            };
        }
    }
}