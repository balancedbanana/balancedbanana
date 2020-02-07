//
// Created by Niklas on 16.01.2020.
//

#include <configfiles/ApplicationConfig.h>

using namespace balancedbanana::configfiles;

ApplicationConfig::ApplicationConfig() = default;

ApplicationConfig::ApplicationConfig(const std::filesystem::path &path) {
    //TODO implement
}

ApplicationConfig::ApplicationConfig(std::istream &data) {
    //TODO implement
}

ApplicationConfig::ApplicationConfig(const balancedbanana::configfiles::ApplicationConfig &original) = default;

void ApplicationConfig::Serialize(std::ostream &data) const{

}

bool ApplicationConfig::Save(const std::filesystem::path &path) const{
    //TODO implement
    return false;
}