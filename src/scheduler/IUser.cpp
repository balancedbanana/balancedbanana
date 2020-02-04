#include <scheduler/IUser.h>

balancedbanana::scheduler::IUser::IUser(const std::string &username, const std::string &pubkey) {
    this->name_ = username;
    this->pubkey_ = pubkey;
};

const std::string &balancedbanana::scheduler::IUser::name() {
    return name_;
}

const std::string &balancedbanana::scheduler::IUser::pubkey() {
    return pubkey_;
}