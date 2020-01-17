#pragma once
#include <string>

namespace balancedbanana {
    namespace Scheduler {

        class IUser {
        private:
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