#pragma once

#include <memory>
#include <QSqlDatabase>

namespace balancedbanana{
    namespace database{
        class IGateway{
        protected:
            std::string dbName;
            IGateway(std::string dbName);
        };
    }
}