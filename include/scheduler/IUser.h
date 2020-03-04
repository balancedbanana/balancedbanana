#pragma once
#include <string>

namespace balancedbanana {
    namespace scheduler {
        class IUser {
        protected:
            //This attribute specifies the name of the user.
            std::string name_;
            std::string pubkey_;

        public:
            IUser() = default;
            IUser(const std::string& username, const std::string& pubkey);
            //Getter for the name_ attribute
            const std::string& name();
            const std::string& pubkey();
        };
    }
}