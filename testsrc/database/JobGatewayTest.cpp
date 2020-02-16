#include <gtest/gtest.h>
#include <database/JobGateway.h>
#include <database/job_result.h>
#include <database/job_details.h>
#include <database/Repository.h>

#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>

using namespace balancedbanana::database;

/**
 * Test environment that has a global SetUp and TearDown methods for all test suites/tests.
 *
 * Sets up the database connection.
 *
 */
class JobGatewayEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        Repository("127.0.0.1", "balancedbanana", "root", "banana", 3306);
    }
};

::testing::Environment* const job_env = ::testing::AddGlobalTestEnvironment(new JobGatewayEnvironment);

/**
 * Deletes the all records in the jobs table and resets the auto increment for the id.
 */
void resetJobTable() {
    QSqlQuery query("ALTER TABLE jobs CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL");
    query.exec();
    query.prepare("DELETE FROM jobs");
    query.exec();
    query.prepare("ALTER TABLE jobs CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT");
    query.exec();
}

/**
 * Fixture class that initializes a sample job's details.
 */
class AddJobTest : public ::testing::Test {
protected:
    void SetUp() override {
        details.id = 1;
    }

    void TearDown() override {
        resetJobTable();
    }

    job_details details;
};

/**
 * Checks if the add query was successful.
 * @param details The record details that were added with the query.
 * @param id The id of the added record.
 * @return true if the add was successful, otherwise false.
 */
bool wasJobAddSuccessful(const job_details& details, uint64_t id){
    QSqlQuery query("SELECT * FROM users WHERE id = ?");
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        if (query.next()){
            int nameIndex = query.record().indexOf("name");
            int emailIndex = query.record().indexOf("email");
            int keyIndex = query.record().indexOf("public_key");
            EXPECT_EQ(query.value(nameIndex).toString().toStdString(), details.name);
            EXPECT_EQ(query.value(emailIndex).toString().toStdString(), details.email);
            EXPECT_EQ(query.value(keyIndex).toString().toStdString(), details.public_key);
            return true;
        } else {
            qDebug() << "record not found";
            return false;
        }
    } else {
        qDebug() << "query failed" << query.lastError();
        return false;
    }

}


// Test checks if the addJob method works properly given correct parameters
TEST_F(AddJobTest, AddJobTest_AddFirstUserSuccess_Test){

    // The first entry's id should be 1
    ASSERT_TRUE(JobGateway::add(details) == 1);

    // The add must be successful
    ASSERT_TRUE(wasUserAddSuccessful(details, 1));
}
