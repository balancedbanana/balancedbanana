#include <database/IGateway.h>

using namespace balancedbanana::database;

IGateway::IGateway(QSqlDatabase db) : db(std::move(db)) {
}
