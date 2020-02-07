//
// Created by Niklas on 16.01.2020.
//

#include <configfiles/ApplicationConfig.h>

using namespace balancedbanana::configfiles;

ApplicationConfig::ApplicationConfig() :
    values_(){
}

ApplicationConfig::ApplicationConfig(const std::filesystem::path &path) {
    //TODO implement
}

size_t ApplicationConfig::count() {
    return values_.size();
}

void ApplicationConfig::Set(const std::string &key, const std::string &value) {
    if(!key.empty()) {
        values_[key] = value;
    }
}

std::string &ApplicationConfig::Get(const std::string &key) {
    return values_[key];
}

void ApplicationConfig::Clear() {
    values_.clear();
}

bool ApplicationConfig::Save(const std::filesystem::path &path) {
    //TODO implement
    return false;
}

bool ApplicationConfig::Contains(const std::string &key) {
    return values_.find(key) != values_.end();
}