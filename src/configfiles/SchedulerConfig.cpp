//
// Created by Niklas on 16.01.2020.
//

#include <configfiles/SchedulerConfig.h>

using namespace balancedbanana::configfiles;

SchedulerConfig::SchedulerConfig() :
    values_(){
}

SchedulerConfig::SchedulerConfig(const std::experimental::filesystem::path &path) {
    //TODO implement
}

size_t SchedulerConfig::count() {
    return values_.size();
}

void SchedulerConfig::Set(const std::string &key, const std::string &value) {
    if(!key.empty()) {
        values_[key] = value;
    }
}

std::string &SchedulerConfig::Get(const std::string &key) {
    return values_[key];
}

void SchedulerConfig::Clear() {
    values_.clear();
}

bool SchedulerConfig::Save(const std::experimental::filesystem::path &path) {
    //TODO implement
    return false;
}

bool SchedulerConfig::Contains(const std::string &key) {
    return values_.find(key) != values_.end();
}