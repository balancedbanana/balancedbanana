//
// Created by Niklas on 16.01.2020.
//

#include <configfiles/ApplicationConfig.h>
#include <fstream>

using namespace balancedbanana::configfiles;

//Helper methods

void readFromStream(ApplicationConfig *config, std::istream *stream) {
    std::string line;
    while(std::getline(*stream, line)) {
        size_t separator = line.find_first_of(':');
        if(separator > 0) {
            std::string name = line.substr(0, separator);
            std::string value = line.substr(separator + 1, std::string::npos);
            (*config)[name] = value;
        }
    }
}

//Class implementation

ApplicationConfig::ApplicationConfig() = default;

ApplicationConfig::ApplicationConfig(const std::filesystem::path &path) : unordered_map() {
    if(std::filesystem::exists(path) && std::filesystem::is_regular_file(path)) {
        std::ifstream stream(path);
        readFromStream(this, &stream);
        stream.close();
    }
}

ApplicationConfig::ApplicationConfig(std::istream &data) : unordered_map(){
    readFromStream(this, &data);
}

ApplicationConfig::ApplicationConfig(const balancedbanana::configfiles::ApplicationConfig &original) = default;

void ApplicationConfig::Serialize(std::ostream &data) const{
    for(const auto &policy : *this) {
        data << policy.first << ":" << policy.second << '\n';
    }
    data.flush();
}

bool ApplicationConfig::Save(const std::filesystem::path &path) const{
    std::ofstream stream(path);
    Serialize(stream);
    stream.close();
    return !stream.bad() && !stream.fail();
}