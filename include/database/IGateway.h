#pragma once

#include <string>
#include <cinttypes>
#include <mutex>
#include <QThreadStorage>
#include <QCache>
#include <QSqlDatabase>

namespace balancedbanana{
    namespace database{
        class IGateway{
            struct Connection{
                std::string host_name;
                std::string databasename;
                std::string username;
                std::string password;
                uint64_t port;
            };
            static std::mutex sync;
            static Connection connection;
            static QThreadStorage<QSqlDatabase> caches;
        public:
            static QSqlDatabase AquireDatabase();
            virtual ~IGateway(){};
            static void createDBConnection(const std::string& host_name, const std::string& databasename, const
            std::string& username, const std::string& password,  uint64_t port);
        };
    }
}