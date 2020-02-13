#include <string>

bool doesTableExist(std::string table_name);
void throwNoTableException(std::string table_name);
bool doesRecordExist(std::string table_name, uint64_t id);