#pragma once

#include <string>
#include <QSqlDatabase>
#include "Specs.h"
#include "worker_details.h"
#include "job_details.h"
#include "user_details.h"

#include <QVariant>

namespace balancedbanana::database {
    class Utilities {
    public:
        /**
         * Checks if table_name exists.
         * @return true when it exists, otherwise false.
         */
        static bool doesTableExist(const std::string& table_name, const std::string& dbName);

        /**
         * Throws an exception for when a table doesn't exist.
         * @param table_name The name of the nonexistent table.
         */
        static void throwNoTableException(const std::string& table_name);

        /**
         * Checks if a record with the given id exists in the database.
         * @param table_name The name of the table which contains the record.
         * @param id The id of the record.
         * @return True if the record exists, otherwise false.
         */
        static bool doesRecordExist(const std::string& table_name, uint64_t id, const std::string& dbName);

        /**
         * Serializes a vector of type T into a string.
         * @tparam T The type of the vector.
         * @param vector The vector containing values of type T.
         * @return The serialized vector in string format.
         */
        template<typename T>
        static std::string serializeVector(std::vector<T> vector);

        /**
         * Deserializes a string into a vector of type T.
         * @tparam T The type.
         * @param string The serialized vector in string format.
         * @return The vector represented by the string.
         */
        template<typename T>
        static std::vector<T> deserializeVector(std::string string);

        /**
         * Casts an uint to a std::optional<uint>.
         * @param value The uint.
         * @return Either the value itself if it's valid, otherwise std::nullopt.
         */
        static std::optional<uint> castToOptional(uint value);

        /**
         * Checks if two vectors of type T are equal.
         * @tparam T The type.
         * @param expected The first vector.
         * @param actual The second vector.
         * @return True if the vectors are equal, otherwise false.
         */
        template <typename T>
        static bool areDetailVectorsEqual(std::vector<T> expected, std::vector<T> actual);
    };
}


