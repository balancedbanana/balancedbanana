#include <database/Utilities.h>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <sstream>

using namespace balancedbanana::database;

/**
 * Checks if table_name exists
 * @return true when it exists, otherwise false
 */
bool Utilities::doesTableExist(const std::string& table_name){
    return QSqlDatabase::database().tables().contains(QString::fromStdString(table_name));
}

/**
* Throws an exception for when a table doesn't exist
*/
void Utilities::throwNoTableException(const std::string& table_name){
    throw std::logic_error(table_name + " table doesn't exist");
}

/**
* Checks if a record with the given id exists in the database.
* @param id The id of the record.
* @return True if the record exists, otherwise false.
*/
bool Utilities::doesRecordExist(const std::string& table_name, uint64_t id){
    QSqlQuery query(QString::fromStdString("SELECT id FROM " + table_name + " WHERE id = ?"));
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return query.next();
    }
    throw std::runtime_error(query.lastError().databaseText().toStdString());
}

/**
 * Serializes a vector of type T into a string
 * @tparam T The type
 * @param vector The vector containing values of type T
 * @return The serialized vector in string format
 */
template<typename T>
std::string Utilities::serializeVector(std::vector<T> vector){
    std::stringstream ss;
    std::string serializedVec;
    {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive(vector);
        serializedVec = ss.str();
    }
    return serializedVec;
}

/**
 * Deserializes a string into a vector of type T
 * @tparam T The type
 * @param string The serialized vector in string format
 * @return The vector represented by the string
 */
template<typename T>
std::vector<T> Utilities::deserializeVector(std::string string){
    std::stringstream ss;
    ss << string;
    std::vector<T> deserializedVec;
    {
        cereal::BinaryInputArchive iarchive(ss);
        try {
            iarchive(deserializedVec);
        }
        catch (std::runtime_error error){
            error.what();
        }
    }
    return deserializedVec;
}

/**
 * Casts an uint to a std::optional<uint>
 * @param value The uint
 * @return Either the value itself if it's valid, otherwise std::nullopt
 */
std::optional<uint> Utilities::castToOptional(uint value) {
    if(value > 0) {
        return value;
    } else {
        return std::nullopt;
    }
}

/**
 * Casts a QVariant to a std::optional<QVariant>
 * @param value The QVariant
 * @return Either the value if itself if it's valid, otherwise std::nullopt
 */
std::optional<QVariant> Utilities::castToOptional(QVariant value) {
    if (value.isNull()){
        return std::optional<QVariant> {std::nullopt};
    } else {
        return value;
    }
}

template <typename T>
bool Utilities::areDetailVectorsEqual(std::vector<T> expected, std::vector<T> actual){
    if (expected.size() != actual.size()){
        return false;
    }
    for (int i = 0; i < expected.size(); i++){
        if (!(expected[i] == actual[i])){
            return false;
        }
    }
    return true;
}

template std::string Utilities::serializeVector(std::vector<std::string> vector);
template std::vector<std::string> Utilities::deserializeVector(std::string string);
template bool Utilities::areDetailVectorsEqual(std::vector<user_details> expected, std::vector<user_details> actual);
template bool Utilities::areDetailVectorsEqual(std::vector<job_details> expected, std::vector<job_details> actual);
template bool Utilities::areDetailVectorsEqual(std::vector<worker_details> expected, std::vector<worker_details> actual);

