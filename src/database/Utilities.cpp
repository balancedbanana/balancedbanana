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

template<typename T>
std::optional<T> Utilities::castToOptional(T foo) {
}

template std::string Utilities::serializeVector(std::vector<std::string> vector);
template std::vector<std::string> Utilities::deserializeVector(std::string string);
