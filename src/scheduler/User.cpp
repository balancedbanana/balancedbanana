#include <scheduler/User.h>

using namespace balancedbanana::scheduler;

User::User(uint64_t id, const std::string &username, const std::string &pubkey) : IUser(username, pubkey) {
    id_ = id;
}

uint64_t User::id() const {
    return id_;
}

const std::string& User::email() const {
    return email_;
}

void User::Setemail(const std::string& mail) {
    email_ = mail;
}