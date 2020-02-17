#pragma once

#include <string>
#include <cinttypes>


namespace balancedbanana{
    namespace database{
        class IGateway{
        public:
            virtual ~IGateway(){};
            static void createDBConnection(const std::string& host_name, const std::string& databasename, const
            std::string& username, const std::string& password,  uint64_t port);
        };
    }
}