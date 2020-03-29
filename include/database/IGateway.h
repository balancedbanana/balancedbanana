#pragma once

#include <memory>
#include <QSqlDatabase>

namespace balancedbanana{
    namespace database{
        class IGateway{
        protected:
            QSqlDatabase db;
            explicit IGateway(QSqlDatabase db);
        };
    }
}