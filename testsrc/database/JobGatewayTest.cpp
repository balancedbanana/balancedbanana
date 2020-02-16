#include <gtest/gtest.h>
#include <database/JobGateway.h>
#include <database/job_result.h>
#include <database/job_details.h>
#include <database/Repository.h>
#include <database/Utilities.h>

#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>
#include <QDataStream>
#include <QDateTime>

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
class AddJobMandatoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        details.id = 1;
        details.status = 1; //scheduled
        details.user_id = 1;
        details.command = "mkdir build";
        details.schedule_time = QDateTime::currentDateTime();
        details.empty = true;
        details.config.set_email("example@kit.edu");
        details.config.set_image("CentOS7");
        details.config.set_current_working_dir("/usr/local/projects/");
    }

    void TearDown() override {
        resetJobTable();
    }

    job_details details;
};

bool areJobDetailsEqual(job_details& first, job_details &second){

}

/**
 * Checks if two vectors of worker_details are equal
 * @param expected The first vector
 * @param actual The second vectors
 * @return true if the vectors are equal, otherwise false
 */
bool areDetailVectorsEqual(std::vector<job_details> expected, std::vector<job_details> actual){
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


/**
 * Checks if the add query was successful, when all values are added.
 * @param details The record details that were added with the query.
 * @param id The id of the added record.
 * @return true if the add was successful, otherwise false.
 */
bool wasJobAddSuccessful(job_details& details, uint64_t id){
    QSqlQuery query("SELECT * FROM users WHERE id = ?");
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        if (query.next()){
            // Get the column index
            int user_id_index = query.record().indexOf("user_id");
            int min_ram_index = query.record().indexOf("min_ram");
            int max_ram_index = query.record().indexOf("max_ram");
            int min_cores_index = query.record().indexOf("min_cores");
            int max_cores_index = query.record().indexOf("max_cores");
            int blocking_mode_index = query.record().indexOf("blocking_mode");
            int email_index = query.record().indexOf("email");
            int priority_index = query.record().indexOf("priority");
            int image_index = query.record().indexOf("image");
            int interruptible_index = query.record().indexOf("interruptible");
            int environment_index = query.record().indexOf("environment");
            int dir_index = query.record().indexOf("working_dir");
            int command = query.record().indexOf("command");
            int schedule_time_index = query.record().indexOf("schedule_time");
            int finish_time = query.record().indexOf("finish_time");
            int start_time = query.record().indexOf("start_time");
            int worker_id_index = query.record().indexOf("worker_id");
            int status_id_index = query.record().indexOf("status_id");
            int allocated_id_index = query.record().indexOf("allocated_id");

            EXPECT_EQ(details.user_id, query.value(user_id_index).toUInt());
            EXPECT_EQ(details.config.min_ram().value(), query.value(min_ram_index).toUInt());
            EXPECT_EQ(details.config.max_ram().value(), query.value(max_ram_index).toUInt());
            EXPECT_EQ(details.config.min_cpu_count().value(), query.value(min_cores_index).toUInt());
            EXPECT_EQ(details.config.max_cpu_count().value(), query.value(max_cores_index).toUInt());
            EXPECT_EQ(details.config.blocking_mode().value(), query.value(blocking_mode_index).toBool());
            EXPECT_EQ(details.config.email(), query.value(email_index).toString().toStdString());
            EXPECT_EQ((uint) details.config.priority().value(), query.value(priority_index).toUInt());
            EXPECT_EQ(details.config.image(), query.value(image_index).toString().toStdString());
            EXPECT_EQ(details.config.interruptible().value(), query.value(interruptible_index).toBool());
//            EXPECT_EQ(details.config.environment().value(), )




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


// Test checks if the addJob method works properly given only the mandatory args.
TEST_F(AddJobMandatoryTest, AddJobMandatoryTest_FirstJobSuccess_Test){

    // The first entry's id should be 1
    ASSERT_TRUE(JobGateway::add(details) == 1);

    // The add must be successful
    ASSERT_TRUE(wasJobAddSuccessful(details, 1));
}
