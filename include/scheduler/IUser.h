#pragma once
#include <string>

namespace balancedbanana {
    namespace scheduler {
        class IUser {
            //This attribute specifies the name of the user.
            std::string name_;
            std::string pubkey_;

        public:
            //Getter for the name_ attribute
            std::string name();
            std::string pubkey();
        };
    }
}