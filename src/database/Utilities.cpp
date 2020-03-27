#include <database/Utilities.h>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <sstream>

using namespace balancedbanana::database;

/**
 * Checks if table_name exists
 * @return true when it exists, otherwise false
 */
bool Utilities::doesTableExist(const std::string& table_name, const std::string& dbName){
    return QSqlDatabase::database(QString::fromStdString(dbName)).tables().contains(QString::fromStdString
    (table_name));
}


void Utilities::throwNoTableException(const std::string& table_name){
    throw std::logic_error(table_name + " table doesn't exist");
}

/**
* Checks if a record with the given id exists in the database.
* @param id The id of the record.
* @return True if the record exists, otherwise false.
*/
bool Utilities::doesRecordExist(const std::string& table_name, uint64_t id, const std::string& dbName){
    QSqlQuery query(QString::fromStdString("SELECT id FROM " + table_name + " WHERE id = ?"), QSqlDatabase::database(QString::fromStdString(dbName)));
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return query.next();
    } else {
        return false;
    }

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


std::optional<uint> Utilities::castToOptional(uint value) {
    if(value > 0) {
        return std::make_optional(value);
    } else {
        return std::nullopt;
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

