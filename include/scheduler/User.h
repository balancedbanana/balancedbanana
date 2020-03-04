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
            User(uint64_t id, const std::string &username, const std::string &pubkey);

            //Getter for the id_ attribute
            uint64_t id() const;

            //getter for the email_ attribute
            const std::string& email() const;

            //setter for the email_ attribute, might be added later
            void Setemail(const std::string& mail);
        };
    }
}