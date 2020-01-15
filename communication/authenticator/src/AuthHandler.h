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
			public:
				static AuthHandler GetDefault();

				AuthHandler();

				virtual  authenticate(const std::shared_ptr<IUser>& user, const std::string& password) = 0;

				void authenticate(const std::shared_ptr<IUser>& user, const std::string& signature);
            };
        }
    }
}