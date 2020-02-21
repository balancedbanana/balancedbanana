#pragma once

#include <string>
#include "Specs.h"
#include "worker_details.h"
#include "job_details.h"
#include "user_details.h"

#include <QVariant>

namespace balancedbanana::database {
    class Utilities {
    public:
        static bool doesTableExist(const std::string& table_name);
        static void throwNoTableException(const std::string& table_name);
        static bool doesRecordExist(const std::string& table_name, uint64_t id);
        template<typename T>
        static std::string serializeVector(std::vector<T> vector);
        template<typename T>
        static std::vector<T> deserializeVector(std::string string);
        static std::optional<uint> castToOptional(uint value);
        static std::optional<QVariant> castToOptional(QVariant value);
        template <typename T>
        static bool areDetailVectorsEqual(std::vector<T> expected, std::vector<T> actual);
    };
}


