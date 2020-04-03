#pragma once

#include <memory>
#include <QSqlDatabase>
#include <stdexcept>

namespace balancedbanana{
    namespace database{
        struct entry_not_exists_error : public std::runtime_error{

            entry_not_exists_error(const std::string &err = "") : std::runtime_error(err) {
            }

        };

        class IGateway{
        protected:
            QSqlDatabase db;
            explicit IGateway(QSqlDatabase db);
        };
    }
}