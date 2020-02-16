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
class AddAllJobTest : public ::testing::Test {
protected:
    void SetUp() override {
        details.id = 1;
        details.status = 1; //scheduled
        details.user_id = 1;
        details.command = "mkdir build";
        details.schedule_time = QDateTime::currentDateTime();
        details.empty = false;
        details.config.set_min_ram(123456);
        details.config.set_max_ram(654321);
        details.config.set_min_cpu_count(42);
        details.config.set_max_cpu_count(43);
        details.config.set_blocking_mode(true);
        details.config.set_email("mail@test.com");
        details.config.set_priority(Priority::low);
        details.config.set_image("testimage");
        details.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        details.config.set_interruptible(false);
        details.config.set_current_working_dir(".");
        details.allocated_specs->ram = 1324;
        details.allocated_specs->cores = 4;
        details.allocated_specs->space = 55;
    }

    void TearDown() override {
        resetJobTable();
    }

    job_details details;
};


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
    QSqlQuery query("SELECT * FROM jobs WHERE id = ?");
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
            int command_index = query.record().indexOf("command");
            int schedule_time_index = query.record().indexOf("schedule_time");
            //int finish_time_index = query.record().indexOf("finish_time");
            //int start_time_index = query.record().indexOf("start_time");
            int worker_id_index = query.record().indexOf("worker_id");
            int status_id_index = query.record().indexOf("status_id");
            int allocated_id_index = query.record().indexOf("allocated_id");

            job_details queryDetails{};
            queryDetails.user_id = query.value(user_id_index).toUInt();
            queryDetails.config.set_min_ram(query.value(min_ram_index).toUInt());
            queryDetails.config.set_max_ram(query.value(max_ram_index).toUInt());
            queryDetails.config.set_min_cpu_count(query.value(min_cores_index).toUInt());
            queryDetails.config.set_max_cpu_count(query.value(max_cores_index).toUInt());
            queryDetails.config.set_blocking_mode(query.value(blocking_mode_index).toBool());
            queryDetails.config.set_email(query.value(email_index).toString().toStdString());
            queryDetails.config.set_priority(static_cast<Priority> (query.value(priority_index).toUInt()));
            queryDetails.config.set_image(query.value(image_index).toString().toStdString());
            queryDetails.config.set_interruptible(query.value(interruptible_index).toBool());
            queryDetails.config.set_environment(Utilities::deserializeVector<std::string>(query.value(environment_index)
            .toString().toStdString()));
            queryDetails.config.set_current_working_dir(query.value(dir_index).toString().toStdString());
            queryDetails.command = query.value(command_index).toString().toStdString();
            queryDetails.schedule_time = QDateTime::fromString(query.value(schedule_time_index).toString(), "yyyy.MM.dd.hh.mm:ss.z");
            //queryDetails.finish_time = QDateTime::fromString(query.value(finish_time_index).toString());
            //queryDetails.start_time = QDateTime::fromString(query.value(start_time_index).toString());
            queryDetails.worker_id = query.value(worker_id_index).toUInt();
            queryDetails.status = query.value(status_id_index).toInt();
            queryDetails.empty = false;

            if (!query.value(allocated_id_index).isNull()){
                query.prepare("SELECT space, cores, ram FROM users WHERE id = ?");
                query.addBindValue(query.value(allocated_id_index).toUInt());
                if (!query.exec()){
                    qDebug() << "allocated_resources query failed";
                } else {
                    if (query.next()){
                        queryDetails.allocated_specs->space = query.value(1).toUInt();
                        queryDetails.allocated_specs->cores = query.value(2).toUInt();
                        queryDetails.allocated_specs->ram = query.value(3).toUInt();
                    }
                }
            }
/*
 *  Keeping this for debugging
            EXPECT_TRUE(queryDetails.user_id == details.user_id);
            EXPECT_TRUE(queryDetails.status == details.status);
            EXPECT_TRUE(queryDetails.schedule_time == details.schedule_time);
            EXPECT_TRUE(queryDetails.finish_time == details.finish_time);
            EXPECT_TRUE(queryDetails.start_time == details.start_time);
            EXPECT_TRUE(((!queryDetails.allocated_specs.has_value() && !details.allocated_specs.has_value()) ||
                         (queryDetails.allocated_specs.value() == details.allocated_specs.value())));
            EXPECT_TRUE(queryDetails.empty == details.empty);
            EXPECT_TRUE(queryDetails.config.min_ram() == details.config.min_ram());
            EXPECT_TRUE(queryDetails.config.max_ram() == details.config.max_ram());
            EXPECT_TRUE(queryDetails.config.min_cpu_count() == details.config.min_cpu_count());
            EXPECT_TRUE(queryDetails.config.max_cpu_count() == details.config.max_cpu_count());
            EXPECT_TRUE(queryDetails.config.blocking_mode() == details.config.blocking_mode());
            EXPECT_TRUE(queryDetails.config.email() == details.config.email());
            EXPECT_TRUE(queryDetails.config.priority() == details.config.priority());
            EXPECT_TRUE(queryDetails.config.image() == details.config.image());
            EXPECT_TRUE(queryDetails.config.environment() == details.config.environment());
            EXPECT_TRUE(queryDetails.config.interruptible() == details.config.interruptible());
            EXPECT_TRUE(queryDetails.config.current_working_dir() == details.config.current_working_dir());
            */
            EXPECT_TRUE(queryDetails == details);
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


// Test checks if the addJob method works properly given all the args.
TEST_F(AddAllJobTest, AddAllJobTest_FirstJobSuccess_Test){
    //qDebug() << "details.starttime = " << details.start_time.value().toString();

    u_int64_t id = JobGateway::add(details);
    qDebug() << id;
    // The first entry's id should be 1
    ASSERT_TRUE(id == 1);

    // The add must be successful
    ASSERT_TRUE(wasJobAddSuccessful(details, 1));
}
