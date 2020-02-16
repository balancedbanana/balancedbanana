#pragma once

#include <string>
#include "Specs.h"
#include "worker_details.h"
#include "job_details.h"
#include "user_details.h"

namespace balancedbanana::database {
    bool doesTableExist(const std::string& table_name);
    void throwNoTableException(const std::string& table_name);
    bool doesRecordExist(const std::string& table_name, uint64_t id);
}


