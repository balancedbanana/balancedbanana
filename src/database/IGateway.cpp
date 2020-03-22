#include <database/IGateway.h>

using namespace balancedbanana::database;

IGateway::IGateway(std::shared_ptr<QSqlDatabase> db) : db(std::move(db)) {
}
