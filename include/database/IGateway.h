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
            /**
             * Stores connection information.
             */
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
            /**
             * Method to acquire the Database connection.
             * @return A QSqlDatabase object for the connection.
             */
            static QSqlDatabase AcquireDatabase();
            virtual ~IGateway(){};

            /**
             * Creates the database connection.
             * @param host_name The host name.
             * @param databasename The name of the database.
             * @param username The username of the MYSQL account.
             * @param password The password of the MYSQL account.
             * @param port The port.
             */
            static void createDBConnection(const std::string& host_name, const std::string& databasename, const
            std::string& username, const std::string& password,  uint64_t port);
        };
    }
}