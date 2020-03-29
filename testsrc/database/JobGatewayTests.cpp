#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#include <gtest/gtest.h>
#include <database/JobGateway.h>
#include <database/job_result.h>
#include <database/job_details.h>
#include <database/Utilities.h>
#include <database/WorkerGateway.h>

#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>
#include <QDataStream>
#include <QDateTime>
#include <QtCore/QTimeZone>

#include "DatabaseTest.h"

using namespace balancedbanana::database;
#define FOUR_MB (4194304)


using JobGatewayTest = DatabaseTest;

/**
 * Deletes the all records in the jobs table and resets the auto increment for the id.
 */
void resetJobTable(const QSqlDatabase &db) {
    QSqlQuery query("ALTER TABLE jobs CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL", db);
    query.exec();
    query.prepare("DELETE FROM jobs");
    query.exec();
    query.prepare("ALTER TABLE jobs CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT");
    query.exec();
}

/**
 * Fixture class that initializes a sample job's details.
 */
class AddJobTest : public JobGatewayTest {
protected:
    void SetUp() override {
        JobGatewayTest::SetUp();
        details.id = 1;
        details.status = 1; //scheduled
        details.user_id = 1;
        details.command = "mkdir build";
        details.schedule_time = QDateTime::currentDateTime();
        details.empty = false;
        details.config.set_min_ram(4194304);
        details.config.set_max_ram(4194304);
        details.config.set_min_cpu_count(42);
        details.config.set_max_cpu_count(43);
        details.config.set_blocking_mode(true);
        details.config.set_priority(Priority::low);
        details.config.set_image("testimage");
        details.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        details.config.set_interruptible(false);
        details.config.set_current_working_dir(".");
        Specs allocated_specs;
        allocated_specs.ram = 1324;
        allocated_specs.cores = 4;
        allocated_specs.osIdentifier = "55";
        details.allocated_specs = allocated_specs;

        user.id = 1;
        user.public_key = "asdjfascd3";
        user.email = "balanced@banana.kit.edu";
        user.name = "Rakan";
        user.empty = false;
    }

    void TearDown() override {
        resetJobTable(db);
        QSqlQuery query("DELETE FROM users", db);
        query.exec();
    }

    job_details details;
    user_details user;
};

/**
 * Checks if the add query was successful, when all values are added.
 * @param details The record details that were added with the query.
 * @param id The id of the added record.
 * @return true if the add was successful, otherwise false.
 */
bool wasJobAddSuccessful(job_details& details, uint64_t id, const QSqlDatabase &db){
    QSqlQuery query("SELECT * FROM jobs WHERE id = ?", db);
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
            int priority_index = query.record().indexOf("priority");
            int image_index = query.record().indexOf("image");
            int interruptible_index = query.record().indexOf("interruptible");
            int environment_index = query.record().indexOf("environment");
            int dir_index = query.record().indexOf("working_dir");
            int command_index = query.record().indexOf("command");
            int schedule_time_index = query.record().indexOf("schedule_time");
            int finish_time_index = query.record().indexOf("finish_time");
            int start_time_index = query.record().indexOf("start_time");
            int worker_id_index = query.record().indexOf("worker_id");
            int status_id_index = query.record().indexOf("status");
            int allocated_id_index = query.record().indexOf("allocated_id");

            job_details queryDetails{};
            queryDetails.user_id = query.value(user_id_index).toUInt();
            queryDetails.config.set_min_ram(Utilities::castToOptional(query.value(min_ram_index).toUInt()));
            queryDetails.config.set_max_ram(Utilities::castToOptional(query.value(max_ram_index).toUInt()));
            queryDetails.config.set_min_cpu_count(Utilities::castToOptional(query.value(min_cores_index).toUInt()));
            queryDetails.config.set_max_cpu_count(Utilities::castToOptional(query.value(max_cores_index).toUInt()));

            if (!query.value(blocking_mode_index).isNull()){
                queryDetails.config.set_blocking_mode(query.value(blocking_mode_index).toBool());
            } else {
                queryDetails.config.set_blocking_mode(std::nullopt);
            }

            queryDetails.config.set_priority(static_cast<Priority> (query.value(priority_index).toUInt()));
            queryDetails.config.set_image(query.value(image_index).toString().toStdString());

            if (!query.value(interruptible_index).isNull()){
                queryDetails.config.set_interruptible(query.value(interruptible_index).toBool());
            } else {
                queryDetails.config.set_interruptible(std::nullopt);
            }

            if(query.value(environment_index).isNull()){
                queryDetails.config.set_environment(std::nullopt);
            } else {
                queryDetails.config.set_environment(Utilities::deserializeVector<std::string>(query.value(environment_index).toString().toStdString()));
            }

            queryDetails.config.set_current_working_dir(query.value(dir_index).toString().toStdString());
            queryDetails.command = query.value(command_index).toString().toStdString();
            queryDetails.schedule_time = QDateTime::fromString(query.value(schedule_time_index).toString(),
                    TIME_FORMAT);
            if (query.value(worker_id_index).isNull()){
                queryDetails.worker_id = std::nullopt;
            } else {
                queryDetails.worker_id = query.value(worker_id_index).toUInt();
            }
            queryDetails.status = query.value(status_id_index).toInt();
            queryDetails.empty = false;

            if (!query.value(allocated_id_index).isNull()){
                query.prepare("SELECT osIdentifier, cores, ram FROM users WHERE id = ?");
                query.addBindValue(query.value(allocated_id_index).toUInt());
                if (!query.exec()){
                    qDebug() << "allocated_resources query failed";
                } else {
                    if (query.next()){
                        Specs allocated_specs{};
                        allocated_specs.osIdentifier = query.value(0).toString().toStdString();
                        allocated_specs.cores = query.value(1).toUInt();
                        allocated_specs.ram = query.value(2).toUInt();
                        queryDetails.allocated_specs = allocated_specs;
                    }
                }
            } else {
                queryDetails.allocated_specs = std::nullopt;
            }
            if (query.value(start_time_index).isNull()){
                queryDetails.start_time = std::nullopt;
            } else {
                queryDetails.start_time = QDateTime::fromString(query.value(start_time_index).toString(),
                        TIME_FORMAT);
            }

            if (query.value(finish_time_index).isNull()){
                queryDetails.finish_time = std::nullopt;
            } else {
                queryDetails.finish_time = QDateTime::fromString(query.value(finish_time_index).toString(),
                                                                TIME_FORMAT);
            }
            /*
            // Keeping this for debugging
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
            EXPECT_TRUE(queryDetails.config.priority() == details.config.priority());
            EXPECT_TRUE(queryDetails.config.image() == details.config.image());
            EXPECT_TRUE(queryDetails.config.environment() == details.config.environment());
            EXPECT_TRUE(queryDetails.config.interruptible() == details.config.interruptible());
            EXPECT_TRUE(queryDetails.config.current_working_dir() == details.config.current_working_dir());
            EXPECT_TRUE(queryDetails.worker_id = details.worker_id);
             */
            //EXPECT_TRUE(queryDetails == details);
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
TEST_F(AddJobTest, AddJobTest_FirstJobSuccess_Test){

    ASSERT_NO_THROW(userGateway->addUser(user));

    // The first entry's id should be 1
    EXPECT_TRUE(jobGateway->addJob(details) == 1);

    // The add must be successful
    EXPECT_TRUE(wasJobAddSuccessful(details, 1, db));
}

// Test to see if the auto increment feature works as expected.
TEST_F(AddJobTest, AddJobTest_AddSecondJobSucess_Test){

    // Add the job from the first test. Since it's the first job, its id should be 1.
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_TRUE(jobGateway->addJob(details) == 1);
    EXPECT_TRUE(wasJobAddSuccessful(details, 1, db));

    // Initialize another job
    // Note that basically all the fields can be equal
    job_details seconddetails;
    seconddetails.id = 2;
    seconddetails.status = 1; //scheduled
    seconddetails.user_id = 1;
    seconddetails.command = "mkdir build";
    seconddetails.schedule_time = QDateTime::currentDateTime();
    seconddetails.empty = false;
    seconddetails.config.set_min_ram(4194304);
    seconddetails.config.set_max_ram(4194304);
    seconddetails.config.set_min_cpu_count(42);
    seconddetails.config.set_max_cpu_count(43);
    seconddetails.config.set_blocking_mode(true);
    seconddetails.config.set_priority(Priority::low);
    seconddetails.config.set_image("testimage");
    seconddetails.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
    seconddetails.config.set_interruptible(false);
    seconddetails.config.set_current_working_dir(".");
    Specs specs;
    specs.ram = 1324;
    specs.cores = 4;
    specs.osIdentifier = "55";
    seconddetails.allocated_specs = specs;

    EXPECT_TRUE(jobGateway->addJob(seconddetails) == 2);
    EXPECT_TRUE(wasJobAddSuccessful(seconddetails, 2, db));
}

TEST_F(AddJobTest, AddJobTest_NoUser_Test){
    EXPECT_THROW(jobGateway->addJob(details), std::runtime_error);
}

/**
 * Restores the jobs table
 */
void createJobsTable(const QSqlDatabase &db){
    QSqlQuery query("CREATE TABLE IF NOT EXISTS `balancedbanana`.`jobs` (\n"
                    "    `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,\n"
                    "    `min_ram` BIGINT(10) UNSIGNED DEFAULT NULL,\n"
                    "    `start_time` VARCHAR(60) NULL DEFAULT NULL,\n"
                    "    `schedule_time` VARCHAR(60) DEFAULT NULL,\n"
                    "    `finish_time` VARCHAR(60) DEFAULT NULL,\n"
                    "    `command` TEXT NOT NULL,\n"
                    "    `image` VARCHAR(500) NOT NULL,\n"
                    "    `blocking_mode` TINYINT(1) DEFAULT NULL,\n"
                    "    `working_dir` TEXT NOT NULL,\n"
                    "    `allocated_id` BIGINT(10) UNSIGNED DEFAULT NULL,\n"
                    "    `interruptible` TINYINT(1) DEFAULT NULL,\n"
                    "    `environment` TEXT,\n"
                    "    `min_cores` INT(10) UNSIGNED DEFAULT NULL,\n"
                    "    `max_cores` INT(10) UNSIGNED DEFAULT NULL,\n"
                    "    `priority` ENUM('low', 'normal', 'high', 'emergency') NOT NULL DEFAULT 'normal',\n"
                    "    `status` ENUM('scheduled', 'processing', 'paused', 'interrupted', 'finished', 'canceled') NOT "
                    "NULL DEFAULT\n"
                    "        'scheduled',\n"
                    "    `max_ram` BIGINT(10) UNSIGNED DEFAULT NULL,\n"
                    "    `user_id` BIGINT(10) UNSIGNED NOT NULL,\n"
                    "    `worker_id` BIGINT(10) DEFAULT NULL,\n"
                    "    `result_id` BIGINT(10) DEFAULT NULL,\n"
                    "    PRIMARY KEY (`id`),\n"
                    "    UNIQUE INDEX `id_UNIQUE` (`id` ASC),\n"
                    "    UNIQUE INDEX `allocated_id_UNIQUE` (`allocated_id` ASC),\n"
                    "    UNIQUE INDEX `result_id_UNIQUE` (`result_id` ASC)\n"
                    ")\n"
                    "ENGINE=InnoDB\n"
                    "DEFAULT CHARSET=utf8;", db);
    query.exec();
}

void deleteJobsTable(const QSqlDatabase &db) {
    QSqlQuery query("DROP TABLE jobs", db);
    query.exec();
}

/**
 * Fixture class that deletes the jobs table on setup and restores it on teardown.
 */
class NoJobsTableTest : public JobGatewayTest {
protected:
    void SetUp() override {
        JobGatewayTest::SetUp();
        // Deletes the jobs table
        deleteJobsTable(db);

        // Setup the variables needed
        details.id = 1;
        details.status = 1; //scheduled
        details.user_id = 1;
        details.command = "mkdir build";
        details.schedule_time = QDateTime::currentDateTime();
        details.empty = false;
        details.config.set_min_ram(4194304);
        details.config.set_max_ram(4194304);
        details.config.set_min_cpu_count(42);
        details.config.set_max_cpu_count(43);
        details.config.set_blocking_mode(true);
        details.config.set_priority(Priority::low);
        details.config.set_image("testimage");
        details.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        details.config.set_interruptible(false);
        details.config.set_current_working_dir(".");
        Specs allocated_specs{};
        allocated_specs.ram = 1324;
        allocated_specs.cores = 4;
        allocated_specs.osIdentifier = "55";
        details.allocated_specs = allocated_specs;
    }

    void TearDown() override {
        createJobsTable(db);
    }

    job_details details;
};

// Test to see if an exception is thrown when a job is being added, but no jobs' table exists.
TEST_F(NoJobsTableTest, NoJobsTableTest_AddJob_Test){
    EXPECT_THROW(jobGateway->addJob(details), std::logic_error);
}

// Test to see if an exception is thrown when a job is being removed, but no jobs' table exists.
TEST_F(NoJobsTableTest, NoJobsTableTest_RemoveJob_Test){
    EXPECT_THROW(jobGateway->removeJob(details.id), std::logic_error);
}

// Test to see if an exception is thrown when the job getter is called, but no jobs' table exists.
TEST_F(NoJobsTableTest, NoJobsTableTest_GetJob_Test){
    EXPECT_THROW(jobGateway->getJob(details.id), std::logic_error);
}

// Test to see if an exception is thrown when the jobs getter is called, but no jobs' table exists.
TEST_F(NoJobsTableTest, NoJobsTableTest_GetJobs_Test){
    EXPECT_THROW(jobGateway->getJobs(), std::logic_error);
}

/**
 * Fixture class that initializes a sample job with only the mandatory information (a.k.a the optionals don't have
 * values)
 */
class AddJobMandatoryTest : public JobGatewayTest {
protected:
    void SetUp() override {
        JobGatewayTest::SetUp();
        details.id = 1;
        details.status = 1; //scheduled
        details.user_id = 1;
        details.command = "mkdir build";
        details.schedule_time = QDateTime::currentDateTime();
        details.empty = false;
        details.config.set_image("testimage");
        details.config.set_current_working_dir(".");
        details.config.set_priority(Priority::high);

        user.id = 1;
        user.public_key = "asdjfascd3";
        user.email = "balanced@banana.kit.edu";
        user.name = "Rakan";
        user.empty = false;
    }

    void TearDown() override {
        resetJobTable(db);
        QSqlQuery query("DELETE FROM users", db);
        query.exec();
    }

    job_details details;
    user_details user;
};

// First add with only the mandatory information
TEST_F(AddJobMandatoryTest, AddJobMandatoryTest_OnlyMandatory_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_TRUE(jobGateway->addJob(details) == 1);
    EXPECT_TRUE(wasJobAddSuccessful(details, 1, db));
}

/**
 * Check if the remove query on id was successful
 * @param id The id of the removed record.
 * @return  true if remove was successful, otherwise false.
 */
bool wasJobRemoveSuccessful(uint64_t id, const QSqlDatabase &db){
    QSqlQuery query("SELECT * FROM jobs WHERE id = ?", db);
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return !query.next();
    } else {
        qDebug() << "wasJobRemoveSuccessful error: " << query.lastError();
        return false;
    }
}

/**
 * Fixture class that resets the jobs table on teardown
 */
class RemoveJobTest : public JobGatewayTest {
protected:
    void TearDown() override{
        resetJobTable(db);
        QSqlQuery query("DELETE FROM users", db);
        query.exec();
    }
};

// Test if remove works properly
TEST_F(RemoveJobTest, RemoveJobTest_SuccessfulRemove_Test){
    // Add a job
    job_details details{};
    details.id = 1;
    details.status = 1; //scheduled
    details.user_id = 1;
    details.command = "mkdir build";
    details.schedule_time = QDateTime::currentDateTime();
    details.empty = false;
    details.config.set_image("testimage");
    details.config.set_current_working_dir(".");
    details.config.set_priority(Priority::low);

    // Since this is the first job, this has to be true.
    user_details user;
    user.id = 1;
    user.public_key = "asdjfascd3";
    user.email = "balanced@banana.kit.edu";
    user.name = "Rakan";
    user.empty = false;
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_TRUE(jobGateway->addJob(details) == 1);
    EXPECT_TRUE(wasJobAddSuccessful(details, 1, db));

    // This must work
    EXPECT_NO_THROW(jobGateway->removeJob(1));
    EXPECT_TRUE(wasJobRemoveSuccessful(1, db));
}

// Test to see if the remove method fails when it's called with an invalid id.
TEST_F(RemoveJobTest, RemoveJobTest_FailureRemove_Test){
    EXPECT_THROW(jobGateway->removeJob(1), std::runtime_error);
}

/**
 * Fixture class that initializes a sample job on setup and resets the jobs table on teardown
 */
class GetJobTest : public JobGatewayTest {
protected:
    void SetUp() override {
        JobGatewayTest::SetUp();
        // Setup the variables needed
        details.id = 1;
        details.status = 1; //scheduled
        details.user_id = 1;
        details.command = "mkdir build";
        details.schedule_time = QDateTime::currentDateTime();
        details.empty = false;
        details.config.set_min_ram(4194304);
        details.config.set_max_ram(4194305);
        details.config.set_min_cpu_count(42);
        details.config.set_max_cpu_count(43);
        details.config.set_blocking_mode(true);
        details.config.set_priority(Priority::low);
        details.config.set_image("testimage");
        details.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        details.config.set_interruptible(false);
        details.config.set_current_working_dir(".");

        user.id = 1;
        user.public_key = "asdjfascd3";
        user.email = "balanced@banana.kit.edu";
        user.name = "Rakan";
        user.empty = false;
    }

    void TearDown() override {
       resetJobTable(db);
        QSqlQuery query("DELETE FROM users", db);
        query.exec();
    }

    job_details details;
    user_details user;
};

void deleteAllocResTable(const QSqlDatabase &db) {
    QSqlQuery query("DROP TABLE allocated_resources", db);
    query.exec();
}

void createAllocResTable(const QSqlDatabase &db) {
    QSqlQuery query("CREATE TABLE IF NOT EXISTS `balancedbanana`.`allocated_resources`\n"
                    "(\n"
                    "    `id`    BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,\n"
                    "    `osIdentifier` TEXT NOT NULL,\n"
                    "    `ram`   BIGINT(10) UNSIGNED NOT NULL,\n"
                    "    `cores` INT(10) UNSIGNED NOT NULL,\n"
                    "    PRIMARY KEY (`id`),\n"
                    "    UNIQUE INDEX `id_UNIQUE` (`id` ASC)\n"
                    ")\n"
                    "ENGINE = InnoDB\n"
                    "DEFAULT CHARACTER SET = utf8", db);
    query.exec();
}

// Test to see if an exception is thrown when the getter is called but no allocated_resorces table exists
TEST_F(GetJobTest, GetJobTest_NoAllocatedResourcesTable_Test){
    // Deletes the allocated_resources table
    deleteAllocResTable(db);

    EXPECT_THROW(jobGateway->getJob(details.id), std::logic_error);

    // Restore the table
    createAllocResTable(db);
}

void deleteResultsTable(const QSqlDatabase &db) {
    QSqlQuery query("DROP TABLE job_results", db);
    query.exec();
}

void createResultsTable(const QSqlDatabase &db) {
    QSqlQuery query("CREATE TABLE `job_results` (\n"
                  "  `id` bigint(10) unsigned NOT NULL AUTO_INCREMENT,\n"
                  "  `output` text NOT NULL,\n"
                  "  `exit_code` tinyint(3) NOT NULL,\n"
                  "  PRIMARY KEY (`id`),\n"
                  "  UNIQUE KEY `id_UNIQUE` (`id`)\n"
                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8", db);
    query.exec();
}

// Test to see if an exception is thrown when the getter is called but no job_results table exists
TEST_F(GetJobTest, GetJobTest_NoJobResultsTable_Test){
    // Deletes the job_results table
    deleteResultsTable(db);

    EXPECT_THROW(jobGateway->getJob(details.id), std::logic_error);

    // Restore the table
    createResultsTable(db);
}

// Test to see if getter returns empty struct when no job was added
TEST_F(GetJobTest, GetJobTest_NonExistentJob_Test){
    EXPECT_THROW(jobGateway->getJob(details.id), std::runtime_error);
}

// Test to see if getter returns correct struct when a job was added
TEST_F(GetJobTest, GetJobTest_FirstAdd_Test){
    // Add the job. Should work without issues
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_TRUE(jobGateway->addJob(details) == details.id);
    EXPECT_TRUE(wasJobAddSuccessful(details, details.id, db));
    EXPECT_TRUE(jobGateway->getJob(details.id) == details);
}

// Test to see if getter works when only mandatory job members were added
TEST_F(GetJobTest, GetJobTest_MandatoryAdd_Test){
    job_details detailss;
    detailss.id = 1;
    detailss.status = (int) JobStatus::scheduled; //scheduled
    detailss.user_id = 1;
    detailss.command = "mkdir build";
    detailss.schedule_time = QDateTime::currentDateTime();
    detailss.empty = false;
    detailss.config.set_image("testimage");
    detailss.config.set_current_working_dir(".");
    detailss.config.set_priority(Priority::low);
    detailss.config.set_interruptible(false);

    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_TRUE(jobGateway->addJob(detailss) == detailss.id);
    EXPECT_TRUE(wasJobAddSuccessful(detailss, detailss.id, db));
    EXPECT_TRUE(jobGateway->getJob(detailss.id) == detailss);
}

/**
 * Fixture class that initializes three jobs on setup and resets the table on teardown.
 */
class GetJobsTest : public JobGatewayTest {
protected:
    void SetUp() override {
        JobGatewayTest::SetUp();
        // Set up the first job
        first.id = 1;
        first.status = 1; //scheduled
        first.user_id = 1;
        first.command = "mkdir build";
        first.schedule_time = QDateTime::currentDateTime();
        first.empty = false;
        first.config.set_min_ram(4194304);
        first.config.set_max_ram(4194305);
        first.config.set_min_cpu_count(42);
        first.config.set_max_cpu_count(43);
        first.config.set_blocking_mode(true);
        first.config.set_priority(Priority::low);
        first.config.set_image("testimage");
        first.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        first.config.set_interruptible(false);
        first.config.set_current_working_dir(".");

        // Set up the second job
        second.id = 2;
        second.status = 1; //scheduled
        second.user_id = 1;
        second.command = "mkdir build";
        second.schedule_time = QDateTime::currentDateTime();
        second.empty = false;
        second.config.set_min_ram(4194304);
        second.config.set_max_ram(4194305);
        second.config.set_min_cpu_count(42);
        second.config.set_max_cpu_count(43);
        second.config.set_blocking_mode(true);
        second.config.set_priority(Priority::high);
        second.config.set_image("testimage");
        second.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        second.config.set_interruptible(false);
        second.config.set_current_working_dir(".");

        // Set up the third job
        third.id = 3;
        third.status = 1; //scheduled
        third.user_id = 1;
        third.command = "mkdir build";
        third.schedule_time = QDateTime::currentDateTime();
        third.empty = false;
        third.config.set_min_ram(4194304);
        third.config.set_max_ram(4194305);
        third.config.set_min_cpu_count(42);
        third.config.set_max_cpu_count(43);
        third.config.set_blocking_mode(true);
        third.config.set_priority(Priority::emergency);
        third.config.set_image("testimage");
        third.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        third.config.set_interruptible(false);
        third.config.set_current_working_dir(".");

        user.id = 1;
        user.public_key = "asdjfascd3";
        user.email = "balanced@banana.kit.edu";
        user.name = "Rakan";
        user.empty = false;

    }

    void TearDown() override {
        resetJobTable(db);
        QSqlQuery query("DELETE FROM users", db);
        query.exec();
    }

    job_details first;
    job_details second;
    job_details third;
    user_details user;
};

// Test to see if getJobs retrieves a vector of previously added jobs from the database
TEST_F(GetJobsTest, GetJobsTest_SuccessfulGet_Test){
    // Add the jobs. Their ids should match the order of their addition.
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_EQ(jobGateway->addJob(first), first.id);
    EXPECT_EQ(jobGateway->addJob(second), second.id);
    EXPECT_EQ(jobGateway->addJob(third), third.id);

    std::vector<job_details> expectedVector;
    expectedVector.push_back(first);
    expectedVector.push_back(second);
    expectedVector.push_back(third);

    std::vector<job_details> actualVector = jobGateway->getJobs();
    EXPECT_TRUE(Utilities::areDetailVectorsEqual(expectedVector, actualVector));
}

// Test to see if an exception is thrown when the getter is called but no allocated_resorces table exists
TEST_F(GetJobsTest, GetJobsTest_NoAllocatedResourcesTable_Test){
    // Deletes the allocated_resources table
    deleteAllocResTable(db);
    EXPECT_THROW(jobGateway->getJobs(), std::logic_error);

    // Restore the table
    createAllocResTable(db);
}

// Test to see if an exception is thrown when the getter is called but no job_results table exists
TEST_F(GetJobsTest, GetJobsTest_NoJobResultsTable_Test){
    // Deletes the job_results table
    deleteResultsTable(db);

    EXPECT_THROW(jobGateway->getJobs(), std::logic_error);

    // Restore the table
    createResultsTable(db);
}

// Test to see if the getter method returns an empty vector if the jobs table is empty
TEST_F(GetJobsTest, GetJobsTest_NonExistentJobs_Test){
    EXPECT_TRUE(jobGateway->getJobs().empty());
}

/**
 * Resets the allocated_resources table
 */
void resetAllocResTable(const QSqlDatabase &db) {
    QSqlQuery query("ALTER TABLE allocated_resources CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL", db);
    query.exec();
    query.prepare("DELETE FROM allocated_resources");
    query.exec();
    query.prepare("ALTER TABLE allocated_resources CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL "
                  "AUTO_INCREMENT");
    query.exec();
}

/*
 * Fixture class that initializes a worker and job on setup and resets the allocated_resoucres, jobs and workers
 * tables on teardown
 */
class StartJobTest : public JobGatewayTest {
protected:
    void SetUp() override {
        JobGatewayTest::SetUp();
        // Setup Job
        job.id = 1;
        job.status = 1; //scheduled
        job.user_id = 1;
        job.command = "mkdir build";
        job.schedule_time = QDateTime::currentDateTime();
        job.empty = false;
        job.config.set_image("testimage");
        job.config.set_current_working_dir(".");
        job.config.set_priority(Priority::emergency);

        // SetUp Worker
        worker.public_key = "sadfjsaljdf";
        Specs specs{};
        specs.osIdentifier = "10240";
        specs.ram = FOUR_MB;
        specs.cores = 4;
        worker.specs = specs;
        worker.name = "Ubuntu";
        worker.id = 1;
        worker.empty = false;

        user.id = 1;
        user.public_key = "asdjfascd3";
        user.email = "balanced@banana.kit.edu";
        user.name = "Rakan";
        user.empty = false;
    }
    void TearDown() override {
        resetJobTable(db);
        resetAllocResTable(db);
        QSqlQuery query("ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL", db);
        query.exec();
        query.prepare("DELETE FROM workers");
        query.exec();
        query.prepare("ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT");
        query.exec();
        query.prepare("DELETE FROM users");
        query.exec();
    }

    job_details job;
    worker_details worker;
    user_details user;
};

bool wasStartSuccessful(job_details job, worker_details worker, const QSqlDatabase &db){
    uint allocated_id = 1;
    QSqlQuery queryAlloc("SELECT cores, ram, osIdentifier FROM allocated_resources WHERE id = ?",
            db);
    queryAlloc.addBindValue(allocated_id);
    QSqlQuery queryJobs("SELECT allocated_id, status, start_time FROM jobs WHERE id = ?", db);
    queryJobs.addBindValue(QVariant::fromValue(job.id));

    if (queryAlloc.exec()){
        if (queryAlloc.next()){
            EXPECT_EQ(queryAlloc.value(0).toUInt(), worker.specs->cores);
            EXPECT_EQ(queryAlloc.value(1).toUInt(), worker.specs->ram);
            EXPECT_EQ(queryAlloc.value(2).toString().toStdString(), worker.specs->osIdentifier);
        } else {
            return false;
        }
    } else {
        return false;
    }

    if (queryJobs.exec()){
        if (queryJobs.next()){
            EXPECT_EQ(queryJobs.value(0).toUInt(), allocated_id);
            EXPECT_EQ(queryJobs.value(1).toString().toStdString(), status_to_string(JobStatus::processing));
            EXPECT_TRUE(QDateTime::fromString(queryJobs.value(2).toString(), TIME_FORMAT)
                        == job.start_time.value());
        } else {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

// Test to see if successful startJob call sets the values in all tables properly
TEST_F(StartJobTest, StartJobTest_SuccessfulStart_Test){
    // Setup by adding a job and worker to the database
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_TRUE(workerGateway->addWorker(worker) == worker.id);
    EXPECT_TRUE(jobGateway->addJob(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id, db));

    job.start_time = std::make_optional<QDateTime>(QDateTime::currentDateTime());
    jobGateway->startJob(job.id, worker.id, worker.specs.value(), job.start_time.value());

    // Check if the values were set properly
    EXPECT_TRUE(wasStartSuccessful(job, worker, db));
}


// Test to see if exception is thrown when no workers table exists
TEST_F(StartJobTest, StartJobTest_NoWorkersTable_Test){
    QSqlQuery query("DROP TABLE workers", db);
    query.exec();

    job.start_time = QDateTime::currentDateTime();
    EXPECT_THROW(jobGateway->startJob(job.id, worker.id, worker.specs.value(), job.start_time.value()), std::logic_error);
    query.prepare("CREATE TABLE IF NOT EXISTS `balancedbanana`.`workers`\n"
                  "(\n"
                  "    `id`         BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,\n"
                  "    `ram`        BIGINT(10) UNSIGNED NULL DEFAULT NULL,\n"
                  "    `cores`      INT(10) UNSIGNED NULL DEFAULT NULL,\n"
                  "    `osIdentifier`   TEXT NULL DEFAULT NULL,\n"
                  "    `public_key` LONGTEXT NOT NULL,\n"
                  "    `name`       VARCHAR(255) NOT NULL,\n"
                  "    PRIMARY KEY (`id`),\n"
                  "    UNIQUE INDEX `id_UNIQUE` (`id` ASC),\n"
                  "    UNIQUE INDEX `name_UNIQUE` (`name` ASC)\n"
                  ")\n"
                  "ENGINE = InnoDB\n"
                  "DEFAULT CHARACTER SET = utf8");
    query.exec();
}

// Test to see if exception is thrown when no jobs table exists
TEST_F(StartJobTest, StartJobTest_NoJobsTable_Test){
    deleteJobsTable(db);
    job.start_time = QDateTime::currentDateTime();
    EXPECT_THROW(jobGateway->startJob(job.id, worker.id, worker.specs.value(), job.start_time.value()), std::logic_error);
    createJobsTable(db);
}

// Test to see if exception is thrown when no job with the id arg exists in the database
TEST_F(StartJobTest, StartJobTest_NonExistentJob_Test){
    job.start_time = QDateTime::currentDateTime();
    EXPECT_THROW(jobGateway->startJob(job.id, worker.id, worker.specs.value(), job.start_time.value()), std::runtime_error);
}

// Test to see if exception is thrown when no worker with the id arg exists in the database
TEST_F(StartJobTest, StartJobTest_NonExistentWorker_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_TRUE(jobGateway->addJob(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id, db));
    job.start_time = QDateTime::currentDateTime();
    EXPECT_THROW(jobGateway->startJob(job.id, worker.id, worker.specs.value(), job.start_time.value()), std::runtime_error);
}

void resetResultsTable(const QSqlDatabase &db) {
    QSqlQuery query("ALTER TABLE job_results CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL",
                    db);
    query.exec();
    query.prepare("DELETE FROM job_results");
    query.exec();
    query.prepare("ALTER TABLE job_results CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT");
    query.exec();
}

/**
 * Fixture class that initializes sample finish data for a Job on SetUp and resets the jobs and job_results tables on
 * TearDown
 */
class FinishJobTest : public JobGatewayTest {
protected:
    void SetUp() override {
        JobGatewayTest::SetUp();
        // Setup Job
        job.id = 1;
        job.status = 1; //scheduled
        job.user_id = 1;
        job.command = "mkdir build";
        job.schedule_time = QDateTime::currentDateTime();
        job.empty = false;
        job.config.set_image("testimage");
        job.config.set_current_working_dir(".");
        job.config.set_priority(Priority::emergency);

        // Finish information
        result.stdout = "Some detailed info...";
        result.exit_code = 10;

        user.id = 1;
        user.public_key = "asdjfascd3";
        user.email = "balanced@banana.kit.edu";
        user.name = "Rakan";
        user.empty = false;

    }

    void TearDown() override {
        resetJobTable(db);
        resetResultsTable(db);
        QSqlQuery query("DELETE FROM users", db);
        query.exec();
    }

    job_details job;
    worker_details worker;
    job_result result;
    user_details user;
};

bool wasFinishSuccessful(const std::string& stdout, job_details job, int8_t exit_code, const
QSqlDatabase &db){
    uint result_id = 1;
    QSqlQuery queryResult("SELECT output, exit_code FROM job_results WHERE id = ?", db);
    queryResult.addBindValue(result_id);
    QSqlQuery queryJobs("SELECT finish_time, result_id FROM jobs WHERE id = ?", db);
    queryJobs.addBindValue(QVariant::fromValue(job.id));

    if (queryResult.exec()){
        if (queryResult.next()){
            EXPECT_EQ(queryResult.value(0).toString().toStdString(), stdout);
            EXPECT_EQ(queryResult.value(1).toInt(), exit_code);
        } else {
            return false;
        }
    } else {
        return false;
    }

    if (queryJobs.exec()){
        if (queryJobs.next()){
            EXPECT_TRUE(QDateTime::fromString(queryJobs.value(0).toString(), TIME_FORMAT) == job
                    .finish_time.value());
            EXPECT_EQ(queryJobs.value(1).toUInt(), result_id);
        } else {
            return false;
        }
    } else {
        return false;
    }
    return true;
}


// Test to see if successful finishJob call sets the values in all tables properly
TEST_F(FinishJobTest, FinishJobTest_SuccessfulFinish_Test){
    // Add the job to the DB. This operation should be successful
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_TRUE(jobGateway->addJob(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id, db));
    job.finish_time = std::make_optional(QDateTime::currentDateTime());
    jobGateway->finishJob(job.id, job.finish_time.value(), result.stdout, result.exit_code);

    // Check if the values were set properly
    EXPECT_TRUE(wasFinishSuccessful(result.stdout, job, result.exit_code, db));
}

// Test to see if exception is thrown when finishJob is called, but no job_results table exists
TEST_F(FinishJobTest, FinishJobTest_NoJobResultsTable_Test){
    deleteResultsTable(db);
    job.finish_time = std::make_optional(QDateTime::currentDateTime());
    EXPECT_THROW(jobGateway->finishJob(job.id, job.finish_time.value(), result.stdout, result.exit_code), std::logic_error);
    createResultsTable(db);
}

// Test to see if exception is thrown when finishJob is called, but no jobs table exists
TEST_F(FinishJobTest, FinishJobTest_NoJobsTable_Test){
    deleteJobsTable(db);
    job.finish_time = std::make_optional(QDateTime::currentDateTime());
    EXPECT_THROW(jobGateway->finishJob(job.id, job.finish_time.value(), result.stdout, result.exit_code), std::logic_error);
    createJobsTable(db);
}

// Test to see if finishJob returns false, when no job exists with the id arg
TEST_F(FinishJobTest, FinishJobTest_NonExistentJob_Test){
    job.finish_time = QDateTime::currentDateTime();
    EXPECT_THROW(jobGateway->finishJob(job.id, job.finish_time.value(), result.stdout, result.exit_code), std::runtime_error);
}

// Test to see if an exception is thrown when finishJob is called with an invalid finish_time
TEST_F(FinishJobTest, FinishJobTest_InvalidFinishTime_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_TRUE(jobGateway->addJob(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id, db));
    job.finish_time = std::make_optional(QDateTime::fromString("0.13.54.13.01:5.5"));
    EXPECT_THROW(jobGateway->finishJob(job.id, job.finish_time.value(), result.stdout, result.exit_code), std::invalid_argument);
}

void resetWorker(const QSqlDatabase &db){
    QSqlQuery query("ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL",
                    db);
    query.exec();
    query.prepare("DELETE FROM workers");
    query.exec();
    query.prepare("ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT");
    query.exec();
}

/*
 * Fixture class that initializes Job and Worker on Setup and resets all tables besides users on TearDown
 */
class GetJobCompleteTest : public JobGatewayTest {
protected:
    void SetUp() override {
        JobGatewayTest::SetUp();
        // Setup Job
        job.id = 1;
        job.user_id = 1;
        job.command = "mkdir build";
        job.schedule_time = QDateTime::currentDateTime();
        job.empty = false;
        job.config.set_image("testimage");
        job.config.set_current_working_dir(".");
        job.config.set_interruptible(true);
        job.status = (int)JobStatus::scheduled;
        job.start_time = std::nullopt;
        job.allocated_specs = std::nullopt;
        job.finish_time = std::nullopt;
        job.worker_id = std::nullopt;
        job.config.set_priority(Priority::high);

        // Finish information
        result.stdout = "Some detailed info...";
        result.exit_code = 0;


        // SetUp Worker
        worker.public_key = "sadfjsaljdf";
        Specs specs{};
        specs.osIdentifier = "10240";
        specs.ram = FOUR_MB;
        specs.cores = 4;
        worker.specs = specs;
        worker.name = "Ubuntu";
        worker.id = 1;
        worker.empty = false;

        user.id = 1;
        user.public_key = "asdjfascd3";
        user.email = "balanced@banana.kit.edu";
        user.name = "Rakan";
        user.empty = false;
    }

    void TearDown() override {
        resetResultsTable(db);
        resetJobTable(db);
        resetAllocResTable(db);
        resetWorker(db);
        QSqlQuery query("DELETE FROM users", db);
        query.exec();
    }

    job_details job;
    worker_details worker;
    job_result result;
    user_details user;
};

// Getter test for after startJob is called
TEST_F(GetJobCompleteTest, GetJobCompleteTest_AfterStart_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_TRUE(workerGateway->addWorker(worker) == worker.id);
    EXPECT_TRUE(jobGateway->addJob(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id, db));
    job.start_time = QDateTime::currentDateTime();
    job.worker_id = 1;
    job.allocated_specs = worker.specs;
    job.status = (int) JobStatus::processing;
    jobGateway->startJob(job.id, worker.id, worker.specs.value(), job.start_time.value());
    EXPECT_TRUE(wasStartSuccessful(job, worker, db));

    job_details queryDetails = jobGateway->getJob(job.id);
    EXPECT_TRUE(queryDetails == job);
}

// Getter test for after finishJob is called
TEST_F(GetJobCompleteTest, GetJobCompleteTest_AfterFinish_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_TRUE(workerGateway->addWorker(worker) == worker.id);
    EXPECT_TRUE(jobGateway->addJob(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id, db));
    job.start_time = QDateTime::currentDateTime();
    job.worker_id = 1;
    job.allocated_specs = worker.specs;
    job.status = (int) JobStatus::processing;
    jobGateway->startJob(job.id, worker.id, worker.specs.value(), job.start_time.value());
    EXPECT_TRUE(wasStartSuccessful(job, worker, db));
    job.finish_time = QDateTime::currentDateTime().addDays(2);
    job.result = result;
    job.status = (int) JobStatus::finished;
    jobGateway->finishJob(job.id, job.finish_time.value(), result.stdout, result.exit_code);
    EXPECT_TRUE(wasFinishSuccessful(result.stdout, job, result.exit_code, db));

    job_details queryDetails = jobGateway->getJob(job.id);
    EXPECT_TRUE(queryDetails == job);
}

/**
 * Fixture class that initializes three jobs on setup and resets the tables on teardown.
 */
class GetJobsWithWorkerIdTest : public JobGatewayTest {
protected:
    void SetUp() override {
        JobGatewayTest::SetUp();
        // Set up the first job
        first.id = 1;
        first.status = 1; //scheduled
        first.user_id = 1;
        first.command = "mkdir build";
        first.schedule_time = QDateTime::currentDateTime();
        first.empty = false;
        first.config.set_min_ram(4194304);
        first.config.set_max_ram(4194305);
        first.config.set_min_cpu_count(42);
        first.config.set_max_cpu_count(43);
        first.config.set_blocking_mode(true);
        first.config.set_priority(Priority::low);
        first.config.set_image("testimage");
        first.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        first.config.set_interruptible(false);
        first.config.set_current_working_dir(".");

        // Set up the second job
        second.id = 2;
        second.status = 1; //scheduled
        second.user_id = 1;
        second.worker_id = 1;
        second.command = "mkdir build";
        second.schedule_time = QDateTime::currentDateTime();
        second.empty = false;
        second.config.set_min_ram(4194304);
        second.config.set_max_ram(4194305);
        second.config.set_min_cpu_count(42);
        second.config.set_max_cpu_count(43);
        second.config.set_blocking_mode(true);
        second.config.set_priority(Priority::high);
        second.config.set_image("testimage");
        second.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        second.config.set_interruptible(false);
        second.config.set_current_working_dir(".");
        second.start_time = std::make_optional<QDateTime>(QDateTime::fromString("2020.02.20:13.13.13.5", TIME_FORMAT));


        // Set up the third job
        third.id = 3;
        third.status = 1; //scheduled
        third.user_id = 1;
        third.worker_id = 1;
        third.command = "mkdir build";
        third.schedule_time = QDateTime::currentDateTime();
        third.empty = false;
        third.config.set_min_ram(4194304);
        third.config.set_max_ram(4194305);
        third.config.set_min_cpu_count(42);
        third.config.set_max_cpu_count(43);
        third.config.set_blocking_mode(true);
        third.config.set_priority(Priority::emergency);
        third.config.set_image("testimage");
        third.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        third.config.set_interruptible(false);
        third.config.set_current_working_dir(".");
        third.start_time = std::make_optional<QDateTime>(QDateTime::fromString("2020.02.21:13.13.13.5", TIME_FORMAT));

        // SetUp Worker
        worker.public_key = "sadfjsaljdf";
        Specs specs{};
        specs.osIdentifier = "10240";
        specs.ram = 2 *FOUR_MB;
        specs.cores = 4;
        worker.specs = specs;
        worker.name = "Ubuntu";
        worker.id = 1;
        worker.empty = false;

        user.id = 1;
        user.public_key = "asdjfascd3";
        user.email = "balanced@banana.kit.edu";
        user.name = "Rakan";
        user.empty = false;

    }

    void TearDown() override {
        resetJobTable(db);
        resetWorker(db);
        resetAllocResTable(db);
        QSqlQuery query("DELETE FROM users", db);
        query.exec();
    }

    job_details first;
    job_details second;
    job_details third;
    worker_details worker;
    user_details user;
};

// Test to see if getJobsWithWorkerId retrieves a vector of previously added jobs from the database.
// second and third are started, so the method only include them in the returned vector even if other jobs exist in
// the database
TEST_F(GetJobsWithWorkerIdTest, GetJobsWithWorkerIdTest_SuccessfulGet_Test){
    // Add the jobs. Their ids should match the order of their addition.
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_EQ(jobGateway->addJob(first), first.id);
    EXPECT_EQ(jobGateway->addJob(second), second.id);
    EXPECT_EQ(jobGateway->addJob(third), third.id);
    EXPECT_EQ(workerGateway->addWorker(worker), worker.id);
    jobGateway->startJob(second.id, worker.id, worker.specs.value(),
            QDateTime::fromString("2020.02.20:13.13.13.5", TIME_FORMAT));
    jobGateway->startJob(third.id, worker.id, worker.specs.value(),
                                     QDateTime::fromString("2020.02.21:13.13.13.5", TIME_FORMAT));
    second.status = (int) JobStatus::processing;
    third.status = (int) JobStatus::processing;
    second.allocated_specs = worker.specs;
    third.allocated_specs = worker.specs;


    std::vector<job_details> expectedVector;
    expectedVector.push_back(second);
    expectedVector.push_back(third);

    std::vector<job_details> actualVector = jobGateway->getJobsWithWorkerId(third.worker_id.value());
    EXPECT_TRUE(Utilities::areDetailVectorsEqual(expectedVector, actualVector));
}

// Test to see if an exception is thrown when the getter is called but no allocated_resources table exists
TEST_F(GetJobsWithWorkerIdTest, GetJobsWithWorkerIdTest_NoAllocatedResourcesTable_Test){
    // Deletes the allocated_resources table
    deleteAllocResTable(db);
    EXPECT_THROW(jobGateway->getJobsWithWorkerId(worker.id), std::logic_error);

    // Restore the table
    createAllocResTable(db);
}

// Test to see if an exception is thrown when the getter is called but no job_results table exists
TEST_F(GetJobsWithWorkerIdTest, GetJobsWithWorkerIdTest_NoJobResultsTable_Test){
    // Deletes the job_results table
    deleteResultsTable(db);

    EXPECT_THROW(jobGateway->getJobsWithWorkerId(worker.id), std::logic_error);

    // Restore the table
    createResultsTable(db);
}

// Test to see if the getter method returns an empty vector if the jobs table is empty
TEST_F(GetJobsWithWorkerIdTest, GetJobsWithWorkerIdTest_NonExistentJobs_Test){
    EXPECT_TRUE(jobGateway->getJobsWithWorkerId(worker.id).empty());
}

/**
 * Creates three jobs and an epoch on SetUp and resets the table on TearDown.
 */
class GetJobsInIntervalTest : public JobGatewayTest {
protected:
    void SetUp() override{
        JobGatewayTest::SetUp();
        // Set up a date/time to work with
        epoch.setDate(QDate(2020, 3, 17));
        epoch.setTime(QTime(20, 30));
        epoch.setTimeZone(QTimeZone::systemTimeZone());

        // Set up the first job
        first.id = 1;
        first.user_id = 1;
        first.command = "mkdir build";
        first.status = (int) JobStatus::scheduled;
        first.schedule_time = epoch.addDays(-7);
        first.empty = false;
        first.config.set_min_ram(4194304);
        first.config.set_max_ram(4194305);
        first.config.set_min_cpu_count(42);
        first.config.set_max_cpu_count(43);
        first.config.set_blocking_mode(true);
        first.config.set_priority(Priority::low);
        first.config.set_image("testimage");
        first.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        first.config.set_interruptible(false);
        first.config.set_current_working_dir(".");

        // Set up the second job
        second.id = 2;
        second.user_id = 1;
        second.command = "mkdir build";
        second.status = (int) JobStatus::scheduled;
        second.schedule_time = epoch.addDays(-8);
        second.empty = false;
        second.config.set_min_ram(4194304);
        second.config.set_max_ram(4194305);
        second.config.set_min_cpu_count(42);
        second.config.set_max_cpu_count(43);
        second.config.set_blocking_mode(true);
        second.config.set_priority(Priority::high);
        second.config.set_image("testimage");
        second.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        second.config.set_interruptible(false);
        second.config.set_current_working_dir(".");


        // Set up the third job
        third.id = 3;
        third.user_id = 1;
        third.command = "mkdir build";
        third.status = (int) JobStatus::scheduled;
        third.schedule_time = epoch.addDays(-2);
        third.empty = false;
        third.config.set_min_ram(4194304);
        third.config.set_max_ram(4194305);
        third.config.set_min_cpu_count(42);
        third.config.set_max_cpu_count(43);
        third.config.set_blocking_mode(true);
        third.config.set_priority(Priority::emergency);
        third.config.set_image("testimage");
        third.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        third.config.set_interruptible(false);
        third.config.set_current_working_dir(".");

        user.id = 1;
        user.public_key = "asdjfascd3";
        user.email = "balanced@banana.kit.edu";
        user.name = "Rakan";
        user.empty = false;
        ASSERT_NO_THROW(userGateway->addUser(user));

        // Standard procedure to add jobs and to get them. This should work flawlessly
        EXPECT_EQ(jobGateway->addJob(first), first.id);
        EXPECT_EQ(jobGateway->addJob(second), second.id);
        EXPECT_EQ(jobGateway->addJob(third), third.id);

        std::vector<job_details> expectedVector;
        expectedVector.push_back(first);
        expectedVector.push_back(second);
        expectedVector.push_back(third);

        std::vector<job_details> actualVector = jobGateway->getJobs();
        EXPECT_TRUE(Utilities::areDetailVectorsEqual(expectedVector, actualVector));

    }

    void TearDown() override {
        resetJobTable(db);
        resetWorker(db);
        resetAllocResTable(db);
        resetResultsTable(db);
        QSqlQuery query("DELETE FROM users", db);
        query.exec();
    }

    job_details first;
    job_details second;
    job_details third;
    QDateTime epoch;
    user_details user;
};

/**
 * Test to see if getJobsInInterval gets the correct scheduled jobs.
 *
 * In this case, the test attempts to retrieve the jobs that were scheduled in the last week. The first job was
 * scheduled one week prior, the second job 8 days prior and the third job 2 days prior.
 */
TEST_F(GetJobsInIntervalTest, GetJobsInIntervalTest_Scheduled_Test){
    std::vector<job_details> actualIntervalJobs = jobGateway->getJobsInInterval(epoch.addDays
            (-7),epoch, JobStatus::scheduled);

    std::vector<job_details> expectedIntervalJobs;
    expectedIntervalJobs.push_back(first);
    expectedIntervalJobs.push_back(third);
    EXPECT_TRUE(Utilities::areDetailVectorsEqual(expectedIntervalJobs, actualIntervalJobs));
}

/**
 * Test to see if getJobsInInterval gets the correct started jobs.
 *
 * In this case, the test attempts to retrieve the jobs that were started within the last 2 days. In this case the
 * first and second job will be started. Only the first job is within the time frame.
 */
TEST_F(GetJobsInIntervalTest, GetJobsInIntervalTest_Started_Test){
    // Add a worker.
    worker_details worker;
    worker.public_key = "sadfjsaljdf";
    Specs sspecs{};
    sspecs.osIdentifier = "10240";
    sspecs.ram = 2 *FOUR_MB;
    sspecs.cores = 4;
    worker.specs = sspecs;
    worker.name = "Ubuntu";
    worker.id = 1;
    worker.empty = false;
    EXPECT_EQ(workerGateway->addWorker(worker), worker.id);
    worker.public_key = "asdfsadcsadcsa";
    worker.name = "Windows";
    EXPECT_EQ(workerGateway->addWorker(worker), worker.id + 1);
    // The job then have to be started.
    first.start_time = epoch.addDays(-1);
    first.status = (int) JobStatus::processing;
    first.allocated_specs = worker.specs;
    first.worker_id = worker.id;
    jobGateway->startJob(first.id, worker.id, worker.specs.value(), first.start_time.value());
    jobGateway->startJob(second.id, worker.id + 1, worker.specs.value(), epoch.addDays
    (-3));

    // This is where the test begins.
    std::vector<job_details> actualIntervalJobs = jobGateway->getJobsInInterval(epoch.addDays
            (-2), epoch, JobStatus::processing);
    std::vector<job_details> expectedIntervalJobs;
    expectedIntervalJobs.push_back(first);
    EXPECT_TRUE(Utilities::areDetailVectorsEqual(expectedIntervalJobs, actualIntervalJobs));
}

/**
 * Test to see if getJobsInInterval gets the correct finished jobs.
 *
 * In this case, the test attempts to retrieve the jobs that were started within the last day. In this case only the
 * third job is finished within the time frame.
 */
TEST_F(GetJobsInIntervalTest, GetJobsInIntervalTest_Finished_Test){
    // Add a worker.
    worker_details worker;
    worker.public_key = "sadfjsaljdf";
    Specs specs{};
    specs.osIdentifier = "10240";
    specs.ram = 2 *FOUR_MB;
    specs.cores = 4;
    worker.specs = specs;
    worker.name = "Ubuntu";
    worker.id = 1;
    worker.empty = false;
    EXPECT_EQ(workerGateway->addWorker(worker), worker.id);
    worker.public_key = "asdfsadcsadcsa";
    worker.name = "Windows";
    EXPECT_EQ(workerGateway->addWorker(worker), worker.id + 1);
    // The job then have to be started.
    first.start_time = epoch.addDays(-1);
    first.status = (int) JobStatus::processing;
    first.allocated_specs = worker.specs;
    first.worker_id = worker.id;
    jobGateway->startJob(first.id, worker.id, worker.specs.value(), first.start_time.value());
    jobGateway->startJob(second.id, worker.id + 1, worker.specs.value(), epoch.addDays
    (-3));

    // Finish the third job
    third.finish_time = epoch.addDays(-1);
    third.status = (int)JobStatus::finished;
    job_result result;
    result.stdout = "error";
    result.exit_code = 0;
    third.result = result;
    jobGateway->finishJob(third.id, third.finish_time.value(), third.result->stdout, third
    .result->exit_code);
     std::vector<job_details> actualIntervalJobs = jobGateway->getJobsInInterval(epoch.addDays
             (-1), epoch, JobStatus::finished);
    std::vector<job_details> expectedIntervalJobs;
    expectedIntervalJobs.push_back(third);
    EXPECT_TRUE(Utilities::areDetailVectorsEqual(expectedIntervalJobs, actualIntervalJobs));
}

// Test to see if method returns empty vector when status is not either processing, finished or scheduled.
TEST_F(GetJobsInIntervalTest, GetJobsInIntervalTest_InvalidStatus_Test){
    EXPECT_TRUE(jobGateway->getJobsInInterval(QDateTime::currentDateTime(), QDateTime::currentDateTime(),
            JobStatus::interrupted).empty());
}

// Test to see if exception is thrown when lower bound of interval is greater than upper bound.
TEST_F(GetJobsInIntervalTest, GetJobsInIntervalTest_InvalidInterval_Test){
    EXPECT_THROW(jobGateway->getJobsInInterval(QDateTime::currentDateTime().addDays(1), QDateTime::currentDateTime(),
            JobStatus::processing), std::invalid_argument);
}

class UpdateJobTest : public JobGatewayTest {
protected:
    void SetUp() override {
        JobGatewayTest::SetUp();
        job.id = 1;
        job.status = (int) JobStatus::scheduled;
        job.user_id = 1;
        job.command = "mkdir build";
        job.schedule_time = QDateTime::currentDateTime();
        job.empty = false;
        job.config.set_min_ram(4194304);
        job.config.set_max_ram(4194305);
        job.config.set_min_cpu_count(42);
        job.config.set_max_cpu_count(43);
        job.config.set_blocking_mode(true);
        job.config.set_priority(Priority::low);
        job.config.set_image("testimage");
        job.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        job.config.set_interruptible(false);
        job.config.set_current_working_dir(".");

        worker.public_key = "sadfjsaljdf";
        Specs specs{};
        specs.osIdentifier = "10240";
        specs.ram = 2 *FOUR_MB;
        specs.cores = 4;
        worker.specs = specs;
        worker.name = "Ubuntu";
        worker.id = 1;
        worker.empty = false;

        user.id = 1;
        user.public_key = "asdjfascd3";
        user.email = "balanced@banana.kit.edu";
        user.name = "Rakan";
        user.empty = false;
    }

    void TearDown() override {
        resetJobTable(db);
        resetAllocResTable(db);
        resetWorker(db);
        QSqlQuery query("DELETE FROM users", db);
        query.exec();
    }

    job_details job;
    worker_details worker;
    user_details user;
};

TEST_F(UpdateJobTest, UpdateJobTest_NoJobsTable_Test){
    deleteJobsTable(db);
    EXPECT_THROW(jobGateway->updateJob(job), std::logic_error);
    createJobsTable(db);
}

TEST_F(UpdateJobTest, UpdateJobTest_NoAllocResourcesTable_Test){
    deleteAllocResTable(db);
    EXPECT_THROW(jobGateway->updateJob(job), std::logic_error);
    createAllocResTable(db);
}

TEST_F(UpdateJobTest, UpdateJobTest_NoResultsTable_Test){
    deleteResultsTable(db);
    EXPECT_THROW(jobGateway->updateJob(job), std::logic_error);
    createResultsTable(db);
}

TEST_F(UpdateJobTest, UpdateJobTest_NoWorkersTable_Test){
    QSqlQuery query("DROP TABLE workers", db);
    query.exec();
    EXPECT_THROW(jobGateway->updateJob(job), std::logic_error);
    query.prepare("CREATE TABLE IF NOT EXISTS `balancedbanana`.`workers`\n"
                  "(\n"
                  "    `id`         BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,\n"
                  "    `ram`        BIGINT(10) UNSIGNED NULL DEFAULT NULL,\n"
                  "    `cores`      INT(10) UNSIGNED NULL DEFAULT NULL,\n"
                  "    `osIdentifier`   TEXT NULL DEFAULT NULL,\n"
                  "    `public_key` LONGTEXT NOT NULL,\n"
                  "    `name`       VARCHAR(255) NOT NULL,\n"
                  "    PRIMARY KEY (`id`),\n"
                  "    UNIQUE INDEX `id_UNIQUE` (`id` ASC),\n"
                  "    UNIQUE INDEX `name_UNIQUE` (`name` ASC)\n"
                  ")\n"
                  "ENGINE = InnoDB\n"
                  "DEFAULT CHARACTER SET = utf8");
    query.exec();
}

TEST_F(UpdateJobTest, UpdateJobTest_InvalidId_Test){
    job.id = 0;
    EXPECT_THROW(jobGateway->updateJob(job), std::invalid_argument);
}

TEST_F(UpdateJobTest, UpdateJobTest_NonExistentJob_Test){
    EXPECT_THROW(jobGateway->updateJob(job), std::runtime_error);
}

TEST_F(UpdateJobTest, UpdateJobTest_UpdateAllocRes_Success_Test){
    // Add the job and worker and then start the job.
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_EQ(jobGateway->addJob(job), job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id, db));
    EXPECT_EQ(workerGateway->addWorker(worker), worker.id);
    job.status = (int) JobStatus::processing;
    job.start_time = QDateTime::currentDateTime();
    job.allocated_specs = worker.specs;
    job.worker_id = 1;
    jobGateway->startJob(job.id, worker.id, worker.specs.value(), job.start_time.value());
    EXPECT_TRUE(wasStartSuccessful(job, worker, db));

    // Change the allocated resources
    Specs new_specs = worker.specs.value();
    new_specs.cores = worker.specs->cores + 2;
    job.allocated_specs = new_specs;
    jobGateway->updateJob(job);

    QSqlQuery query("SELECT allocated_id FROM jobs WHERE id = ?", db);
    query.addBindValue(QVariant::fromValue(job.id));
    query.exec();
    query.next();
    QSqlQuery allocQuery("SELECT cores, ram, osIdentifier FROM allocated_resources WHERE id = ?", db);
    EXPECT_EQ(query.value(0).toUInt(), 1);
    allocQuery.addBindValue(query.value(0));
    allocQuery.exec();
    allocQuery.next();
    EXPECT_EQ(allocQuery.value(0).toUInt(), worker.specs->cores + 2);
    EXPECT_EQ(allocQuery.value(1).toUInt(), worker.specs->ram);
    EXPECT_EQ(allocQuery.value(2).toString().toStdString(), worker.specs->osIdentifier);
}

TEST_F(UpdateJobTest, UpdateJobTest_UpdateAllocRes_NoValue_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_EQ(jobGateway->addJob(job), job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id, db));
    jobGateway->updateJob(job);

    // Nothing changed
    job_details actualJob = jobGateway->getJob(job.id);
    EXPECT_TRUE(job == actualJob);
}

TEST_F(UpdateJobTest, UpdateJobTest_UpdateWorkerId_Test){
    // Add the job and  two workers and then start the job.
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_EQ(jobGateway->addJob(job), job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id, db));
    EXPECT_EQ(workerGateway->addWorker(worker), worker.id);
    worker.public_key = "sadfsdcasd";
    worker.name = "windows";
    EXPECT_EQ(workerGateway->addWorker(worker), worker.id + 1);
    job.status = (int) JobStatus::processing;
    job.start_time = QDateTime::currentDateTime();
    job.allocated_specs = worker.specs;
    job.worker_id = 1;
    jobGateway->startJob(job.id, worker.id, worker.specs.value(), job.start_time.value());
    EXPECT_TRUE(wasStartSuccessful(job, worker, db));

    // Update the worker_id
    job.worker_id = worker.id + 1;
    jobGateway->updateJob(job);
    QSqlQuery query("SELECT worker_id FROM jobs WHERE id = ?", db);
    query.addBindValue(QVariant::fromValue(job.id));
    query.exec();
    query.next();
    EXPECT_EQ(query.value(0).toUInt(), job.worker_id);
}

TEST_F(UpdateJobTest, UpdateJobTest_UpdateStatusInterrupted_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_EQ(jobGateway->addJob(job), job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id, db));
    job.status = (int) JobStatus::interrupted;
    jobGateway->updateJob(job);

    job_details actualJob = jobGateway->getJob(job.id);
    EXPECT_TRUE(job == actualJob);
}

TEST_F(UpdateJobTest, UpdateJobTest_UpdateStatusPaused_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_EQ(jobGateway->addJob(job), job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id, db));
    job.status = (int) JobStatus::paused;
    jobGateway->updateJob(job);

    job_details actualJob = jobGateway->getJob(job.id);
    EXPECT_TRUE(job == actualJob);
}

TEST_F(UpdateJobTest, UpdateJobTest_UpdateStatusCanceled_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_EQ(jobGateway->addJob(job), job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id, db));
    job.status = (int) JobStatus::canceled;
    jobGateway->updateJob(job);

    job_details actualJob = jobGateway->getJob(job.id);
    EXPECT_TRUE(job == actualJob);
}

TEST_F(UpdateJobTest, UpdateJobTest_UpdatePriority_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    EXPECT_EQ(jobGateway->addJob(job), job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id, db));
    job.config.set_priority(Priority::emergency);
    jobGateway->updateJob(job);

    job_details actualJob = jobGateway->getJob(job.id);
    EXPECT_TRUE(job == actualJob);
}

class UpdateJobBypassTest : public JobGatewayTest {
protected:
    void SetUp() override {
        JobGatewayTest::SetUp();
        job.id = 1;
        job.status = (int) JobStatus::scheduled;
        job.user_id = 1;
        job.command = "mkdir build";
        job.schedule_time = QDateTime::currentDateTime();
        job.empty = false;
        job.config.set_min_ram(4194304);
        job.config.set_max_ram(4194305);
        job.config.set_min_cpu_count(42);
        job.config.set_max_cpu_count(43);
        job.config.set_blocking_mode(true);
        job.config.set_priority(Priority::low);
        job.config.set_image("testimage");
        job.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        job.config.set_interruptible(false);
        job.config.set_current_working_dir(".");

        worker.public_key = "sadfjsaljdf";
        Specs specs{};
        specs.osIdentifier = "10240";
        specs.ram = 2 *FOUR_MB;
        specs.cores = 4;
        worker.specs = specs;
        worker.name = "Ubuntu";
        worker.id = 1;
        worker.empty = false;

        user.id = 1;
        user.public_key = "asdjfascd3";
        user.email = "balanced@banana.kit.edu";
        user.name = "Rakan";
        user.empty = false;
    }

    void TearDown() override {
        resetJobTable(db);
        resetAllocResTable(db);
        resetWorker(db);
        resetResultsTable(db);
        QSqlQuery query("DELETE FROM users", db);
        query.exec();
    }

    job_details job;
    worker_details worker;
    user_details user;
};

TEST_F(UpdateJobBypassTest, UpdateJobBypassTest_NoJobsTable_Test){
    deleteJobsTable(db);
    EXPECT_THROW(jobGateway->updateJobBypassWriteProtection(job), std::logic_error);
    createJobsTable(db);
}

TEST_F(UpdateJobBypassTest, UpdateJobBypassTest_NoAllocResTable_Test){
    deleteAllocResTable(db);
    EXPECT_THROW(jobGateway->updateJobBypassWriteProtection(job), std::logic_error);
    createAllocResTable(db);
}

TEST_F(UpdateJobBypassTest, UpdateJobBypassTest_NoResultsTable_Test){
    deleteResultsTable(db);
    EXPECT_THROW(jobGateway->updateJobBypassWriteProtection(job), std::logic_error);
    createResultsTable(db);
}

TEST_F(UpdateJobBypassTest, UpdateJobBypassTest_NoWorkersTable_Test){
    QSqlQuery query("DROP TABLE workers", db);
    query.exec();
    EXPECT_THROW(jobGateway->updateJobBypassWriteProtection(job), std::logic_error);
    query.prepare("CREATE TABLE IF NOT EXISTS `balancedbanana`.`workers`\n"
                  "(\n"
                  "    `id`         BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,\n"
                  "    `ram`        BIGINT(10) UNSIGNED NULL DEFAULT NULL,\n"
                  "    `cores`      INT(10) UNSIGNED NULL DEFAULT NULL,\n"
                  "    `osIdentifier`   TEXT NULL DEFAULT NULL,\n"
                  "    `public_key` LONGTEXT NOT NULL,\n"
                  "    `name`       VARCHAR(255) NOT NULL,\n"
                  "    PRIMARY KEY (`id`),\n"
                  "    UNIQUE INDEX `id_UNIQUE` (`id` ASC),\n"
                  "    UNIQUE INDEX `name_UNIQUE` (`name` ASC)\n"
                  ")\n"
                  "ENGINE = InnoDB\n"
                  "DEFAULT CHARACTER SET = utf8");
    query.exec();
}

TEST_F(UpdateJobBypassTest, UpdateJobBypassTest_InvalidId_Test){
    job.id = 0;
    EXPECT_THROW(jobGateway->updateJobBypassWriteProtection(job), std::invalid_argument);
}

TEST_F(UpdateJobBypassTest, UpdateJobBypassTest_NonExistentJob_Test){
    EXPECT_THROW(jobGateway->updateJobBypassWriteProtection(job), std::runtime_error);
}

// This test is for when both the database and the new job_details don't have allocated resources
TEST_F(UpdateJobBypassTest, UpdateJobBypassTest_UpdateAllocNoAllocResBoth_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    ASSERT_EQ(jobGateway->addJob(job), job.id);
    ASSERT_TRUE(wasJobAddSuccessful(job, job.id, db));

    jobGateway->updateJobBypassWriteProtection(job);
    job_details job_actual = jobGateway->getJob(job.id);
    EXPECT_TRUE(job_actual == job);
    QSqlQuery query("SELECT allocated_id FROM jobs WHERE id = ?", db);
    query.addBindValue(QVariant::fromValue(job.id));
    ASSERT_TRUE(query.exec());
    ASSERT_TRUE(query.next());
    EXPECT_TRUE(query.value(0).isNull());
}

// This test is for when the allocated resources are NULL in the database, but the new job_details has them
TEST_F(UpdateJobBypassTest, UpdateJobBypassTest_UpdateAllocNoAllocResDB_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    ASSERT_EQ(jobGateway->addJob(job), job.id);
    ASSERT_TRUE(wasJobAddSuccessful(job, job.id, db));

    job.allocated_specs = worker.specs;
    jobGateway->updateJobBypassWriteProtection(job);
    job_details job_actual = jobGateway->getJob(job.id);
    EXPECT_TRUE(job_actual == job);

    QSqlQuery queryJobsTable("SELECT allocated_id FROM jobs WHERE id = ?", db);
    queryJobsTable.addBindValue(QVariant::fromValue(job.id));
    ASSERT_TRUE(queryJobsTable.exec());
    ASSERT_TRUE(queryJobsTable.next());
    ASSERT_FALSE(queryJobsTable.value(0).isNull());

    QSqlQuery queryAllocResTable("SELECT osIdentifier, ram, cores FROM allocated_resources WHERE id = ?", db);
    queryAllocResTable.addBindValue(queryJobsTable.value(0));
    ASSERT_TRUE(queryAllocResTable.exec());
    ASSERT_TRUE(queryAllocResTable.next());
    EXPECT_EQ(queryAllocResTable.value(0).toString().toStdString(), job.allocated_specs->osIdentifier);
    EXPECT_EQ(queryAllocResTable.value(1).toInt(), job.allocated_specs->ram);
    EXPECT_EQ(queryAllocResTable.value(2).toInt(), job.allocated_specs->cores);
}

// Test for when Job already has allocated_specs, but specs are updated
TEST_F(UpdateJobBypassTest, UpdateJobBypassTest_UpdateAlloc_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    ASSERT_EQ(jobGateway->addJob(job), job.id);
    ASSERT_TRUE(wasJobAddSuccessful(job, job.id, db));
    ASSERT_TRUE(workerGateway->addWorker(worker));
    job.worker_id = worker.id;
    job.allocated_specs = worker.specs;
    job.start_time = QDateTime::currentDateTime();
    job.status = (int) JobStatus::processing;
    jobGateway->startJob(job.id, job.worker_id.value(), job.allocated_specs.value(), job.start_time.value());
    ASSERT_TRUE(wasStartSuccessful(job, worker, db));

    // Get old AllocId
    QSqlQuery queryAllocId("SELECT allocated_id FROM jobs WHERE id = ?", db);
    queryAllocId.addBindValue(QVariant::fromValue(job.id));
    ASSERT_TRUE(queryAllocId.exec());
    ASSERT_TRUE(queryAllocId.next());

    // Change specs
    job.allocated_specs->ram = worker.specs->ram + 1;
    jobGateway->updateJobBypassWriteProtection(job);

    // Getter should return equal job_details
    job_details job_actual = jobGateway->getJob(job.id);
    EXPECT_TRUE(job_actual == job);

    // AllocId should be unchanged
    QSqlQuery queryAllocIdNew("SELECT allocated_id FROM jobs WHERE id = ?", db);
    queryAllocIdNew.addBindValue(QVariant::fromValue(job.id));
    ASSERT_TRUE(queryAllocIdNew.exec());
    ASSERT_TRUE(queryAllocIdNew.next());
    EXPECT_EQ(queryAllocId.value(0).toInt(), queryAllocIdNew.value(0).toInt());
}

// This test is for when both the database and the new job_details don't have results
TEST_F(UpdateJobBypassTest, UpdateJobBypassTest_UpdateNoResultBoth_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    ASSERT_EQ(jobGateway->addJob(job), job.id);
    ASSERT_TRUE(wasJobAddSuccessful(job, job.id, db));

    jobGateway->updateJobBypassWriteProtection(job);
    job_details job_actual = jobGateway->getJob(job.id);
    EXPECT_TRUE(job_actual == job);
    QSqlQuery query("SELECT result_id FROM jobs WHERE id = ?", db);
    query.addBindValue(QVariant::fromValue(job.id));
    ASSERT_TRUE(query.exec());
    ASSERT_TRUE(query.next());
    EXPECT_TRUE(query.value(0).isNull());
}

// This test is for when the results are NULL in the database, but the new job_details has them
TEST_F(UpdateJobBypassTest, UpdateJobBypassTest_UpdateNoResultDB_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    ASSERT_EQ(jobGateway->addJob(job), job.id);
    ASSERT_TRUE(wasJobAddSuccessful(job, job.id, db));

    job.result = {"some result..", 100};
    jobGateway->updateJobBypassWriteProtection(job);
    job_details job_actual = jobGateway->getJob(job.id);
    EXPECT_TRUE(job_actual == job);

    QSqlQuery queryJobsTable("SELECT result_id FROM jobs WHERE id = ?", db);
    queryJobsTable.addBindValue(QVariant::fromValue(job.id));
    ASSERT_TRUE(queryJobsTable.exec());
    ASSERT_TRUE(queryJobsTable.next());
    ASSERT_FALSE(queryJobsTable.value(0).isNull());

    QSqlQuery queryResultsTable("SELECT output, exit_code FROM job_results WHERE id = ?", db);
    queryResultsTable.addBindValue(queryJobsTable.value(0));
    ASSERT_TRUE(queryResultsTable.exec());
    ASSERT_TRUE(queryResultsTable.next());
    EXPECT_EQ(queryResultsTable.value(0).toString().toStdString(), job.result->stdout);
    EXPECT_EQ(queryResultsTable.value(1).toInt(), job.result->exit_code);
}

// Test for when Job already has results, but results are updated
TEST_F(UpdateJobBypassTest, UpdateJobBypassTest_UpdateResult_Test){
    ASSERT_NO_THROW(userGateway->addUser(user));
    ASSERT_EQ(jobGateway->addJob(job), job.id);
    ASSERT_TRUE(wasJobAddSuccessful(job, job.id, db));
    ASSERT_TRUE(workerGateway->addWorker(worker));
    job.finish_time = QDateTime::currentDateTime();
    job.status = (int) JobStatus::finished;
    job.result = {"some result..", 100};
    jobGateway->finishJob(job.id, job.finish_time.value(), job.result->stdout, job.result->exit_code);
    ASSERT_TRUE(wasFinishSuccessful(job.result->stdout, job, job.result->exit_code, db));

    // Get old ResultId
    QSqlQuery queryResultId("SELECT result_id FROM jobs WHERE id = ?", db);
    queryResultId.addBindValue(QVariant::fromValue(job.id));
    ASSERT_TRUE(queryResultId.exec());
    ASSERT_TRUE(queryResultId.next());

    // Change result
    job.result->exit_code = 10;
    job.result->stdout = "some other result";
    jobGateway->updateJobBypassWriteProtection(job);

    // Getter should return equal job_details
    job_details job_actual = jobGateway->getJob(job.id);
    EXPECT_TRUE(job_actual == job);

    // ResultId should be unchanged
    QSqlQuery queryResultIdNew("SELECT result_id FROM jobs WHERE id = ?", db);
    queryResultIdNew.addBindValue(QVariant::fromValue(job.id));
    ASSERT_TRUE(queryResultIdNew.exec());
    ASSERT_TRUE(queryResultIdNew.next());
    EXPECT_EQ(queryResultId.value(0).toInt(), queryResultIdNew.value(0).toInt());
}