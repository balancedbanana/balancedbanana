#pragma once
#include "AuthHandler.h"

namespace balancedbanana {
    namespace communication {
        namespace authenticator {
            class LDAPAuthHandler : public AuthHandler {
            public:
                virtual void authenticate(const std::shared_ptr<Scheduler::IUser>& user, const std::string& password) override;
            };
        }
    }
}