#pragma once

#include <map>
#include <filesystem>

namespace balancedbanana::configfiles {

//This class contains policies for the scheduler as key-value pairs.
    class ApplicationConfig {
    private:
        //This attribute contains all the policies for the scheduler.
        std::map <std::string, std::string> values_;


    public:
        //This constructor creates an empty ApplicationConfig.
        ApplicationConfig();

        //This constructor loads a ApplicationConfig from a file.
        ApplicationConfig(const std::filesystem::path &path);

        //This method returns the amount of key-value pairs in the ApplicationConfig.
        size_t count();

        //This method adds policy with a specified key to the ApplicationConfig.
        void Set(const std::string &key, const std::string &value);

        //This method returns the policy associated with the specified key. If the ApplicationConfig does not contain such a policy, an empty string is returned.
        std::string &Get(const std::string &key);

        //This method clears out the ApplicationConfig.
        void Clear();

        //This method serializes the ApplicationConfig and saves it to the specified file.
        bool Save(const std::filesystem::path &path);

        //This method returns true when the ApplicationConfig contains a policy associated with the specified key, otherwise it returns false.
        bool Contains(const std::string &key);

    };

}