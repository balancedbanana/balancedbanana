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

				virtual uint64_t authenticate(const std::shared_ptr<scheduler::IUser>& user, const std::string& password) = 0;

				void publickeyauthenticate(const std::shared_ptr<scheduler::IUser>& user, const std::string& signature);
            };
        }
    }
}