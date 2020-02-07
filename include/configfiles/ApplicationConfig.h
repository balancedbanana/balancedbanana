#pragma once

#include <map>
#include <filesystem>

namespace balancedbanana::configfiles {

//This class contains policies for the application as key-value pairs.
    class ApplicationConfig : public std::map<std::string, std::string> {
    public:
        //This constructor creates an empty ApplicationConfig.
        ApplicationConfig();

        //This constructor loads a ApplicationConfig from a file.
        explicit ApplicationConfig(const std::filesystem::path &path);

        explicit ApplicationConfig(std::istream &data);

        ApplicationConfig(const ApplicationConfig &);

        void Serialize(std::ostream &data) const;

        //This method serializes the ApplicationConfig and saves it to the specified file.
        bool Save(const std::filesystem::path &path) const;

    };

}