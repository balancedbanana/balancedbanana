#pragma once
#include "IUser.h"

namespace balancedbanana {
    namespace scheduler {

        class User : public IUser {
        private:
            //This attribute stores the user ID that is used to identify a user
            uint64_t id_;

            //This attribute specifies the email address that should be used to notify the user.
            std::string email_;


        public:
            //Getter for the id_ attribute
            uint64_t id();

            //getter for the email_ attribute
            std::string email();

        };
    }
}