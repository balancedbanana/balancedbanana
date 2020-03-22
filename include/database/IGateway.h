#pragma once

#include <memory>
#include <QSqlDatabase>

namespace balancedbanana{
    namespace database{
        class IGateway{
        protected:
            std::shared_ptr<QSqlDatabase> db;
            explicit IGateway(std::shared_ptr<QSqlDatabase> db);
        };
    }
}