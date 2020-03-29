#pragma once

#include <unordered_map>
#include <filesystem>

namespace balancedbanana::configfiles {

//This class contains policies for the application as key-value pairs.
    class ApplicationConfig : public std::unordered_map<std::string, std::string> {
    public:
        //This constructor creates an empty ApplicationConfig.
        ApplicationConfig();

        //This constructor loads a ApplicationConfig from a file.
        explicit ApplicationConfig(const std::filesystem::path &path);

        explicit ApplicationConfig(std::istream &data);

        ApplicationConfig(const ApplicationConfig &);

        void readFromStream(std::istream &stream);

        bool Contains(const std::string& key);

        void Serialize(std::ostream &data) const;

        //This method serializes the ApplicationConfig and saves it to the specified file.
        bool Save(const std::filesystem::path &path) const;

    };

}