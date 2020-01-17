#pragma once
#include <string>
#include <memory>
#include <scheduler/IUser.h>

namespace balancedbanana {
    namespace communication {
        namespace authenticator {
            class AuthHandler {
			public:
				static std::shared_ptr<AuthHandler> GetDefault();

				virtual void authenticate(const std::shared_ptr<Scheduler::IUser>& user, const std::string& password) = 0;

				void publickeyauthenticate(const std::shared_ptr<Scheduler::IUser>& user, const std::string& signature);
            };
        }
    }
}