#include <gtest/gtest.h>
#include <database/JobGateway.h>
#include <database/job_result.h>
#include <database/job_details.h>
#include <database/Repository.h>
#include <database/Utilities.h>
#include <database/WorkerGateway.h>

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
class AddJobTest : public ::testing::Test {
protected:
    void SetUp() override {
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
            queryDetails.config.set_min_ram(Utilities::castToOptional(query.value(min_ram_index).toUInt()));
            queryDetails.config.set_max_ram(Utilities::castToOptional(query.value(max_ram_index).toUInt()));
            queryDetails.config.set_min_cpu_count(Utilities::castToOptional(query.value(min_cores_index).toUInt()));
            queryDetails.config.set_max_cpu_count(Utilities::castToOptional(query.value(max_cores_index).toUInt()));

            std::optional<QVariant> castedBlocking = Utilities::castToOptional(query.value(blocking_mode_index));
            if (castedBlocking != std::nullopt){
                queryDetails.config.set_blocking_mode(castedBlocking.value().toBool());
            } else {
                queryDetails.config.set_blocking_mode(std::nullopt);
            }
            queryDetails.config.set_email(query.value(email_index).toString().toStdString());

            std::optional<uint> castedPriorityId = Utilities::castToOptional(query.value(priority_index).toUInt());
            if (castedPriorityId != std::nullopt){
                queryDetails.config.set_priority(static_cast<Priority> (query.value(priority_index).toUInt()));
            } else {
                queryDetails.config.set_priority(std::nullopt);
            }
            queryDetails.config.set_image(query.value(image_index).toString().toStdString());

            std::optional<QVariant> castedInterruptible = Utilities::castToOptional(query.value(interruptible_index));
            if (castedInterruptible != std::nullopt){
                queryDetails.config.set_interruptible(castedInterruptible->toBool());
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
                    "yyyy.MM.dd:hh.mm.ss.z");
            queryDetails.worker_id = Utilities::castToOptional(query.value(worker_id_index).toUInt());
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
            EXPECT_TRUE(queryDetails.config.email() == details.config.email());
            EXPECT_TRUE(queryDetails.config.priority() == details.config.priority());
            EXPECT_TRUE(queryDetails.config.image() == details.config.image());
            EXPECT_TRUE(queryDetails.config.environment() == details.config.environment());
            EXPECT_TRUE(queryDetails.config.interruptible() == details.config.interruptible());
            EXPECT_TRUE(queryDetails.config.current_working_dir() == details.config.current_working_dir());
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
TEST_F(AddJobTest, AddAllJobTest_FirstJobSuccess_Test){

    // The first entry's id should be 1
    EXPECT_TRUE(JobGateway::add(details) == 1);

    // The add must be successful
    EXPECT_TRUE(wasJobAddSuccessful(details, 1));
}

// Test to see if the auto increment feature works as expected.
TEST_F(AddJobTest, AddWorkerTest_AddSecondJobSucess_Test){

    // Add the user from the first test. Since it's the first user, its id should be 1.
    EXPECT_TRUE(JobGateway::add(details) == 1);
    EXPECT_TRUE(wasJobAddSuccessful(details, 1));

    // Initialize another job
    // Note that basically all the fields can be equal
    job_details seconddetails{};
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
    seconddetails.config.set_email("mail@test.com");
    seconddetails.config.set_priority(Priority::low);
    seconddetails.config.set_image("testimage");
    seconddetails.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
    seconddetails.config.set_interruptible(false);
    seconddetails.config.set_current_working_dir(".");
    seconddetails.allocated_specs->ram = 1324;
    seconddetails.allocated_specs->cores = 4;
    seconddetails.allocated_specs->space = 55;

    EXPECT_TRUE(JobGateway::add(seconddetails) == 2);
    EXPECT_TRUE(wasJobAddSuccessful(seconddetails, 2));
}

// Test to see if the addJob method throws an exception when the id is invalid.
TEST_F(AddJobTest, AddJobTest_Invalid_Job_id_Test){
    details.user_id = 0;
    EXPECT_THROW(JobGateway::add(details), std::invalid_argument);
}

// Test to see if the addJob method throws an exception when the command arg is invalid.
TEST_F(AddJobTest, AddJobTest_Invalid_Command_Test){
    details.command = "";
    EXPECT_THROW(JobGateway::add(details), std::invalid_argument);
}

// Test to see if the addJob method throws an exception when the email arg is invalid.
TEST_F(AddJobTest, AddJobTest_Invalid_EMAIL_Test){
    details.config.set_email("");
    EXPECT_THROW(JobGateway::add(details), std::invalid_argument);
}

// Test to see if the addJob method throws an exception when the schedule_time arg is invalid.
TEST_F(AddJobTest, AddJobTest_Invalid_Schedule_Time_Test){
    details.schedule_time = QDateTime::fromString("0.13.54.13.01:5.5");
    EXPECT_THROW(JobGateway::add(details), std::invalid_argument);
}

// Test to see if the addJob method throws an exception when the min_ram arg is invalid.
TEST_F(AddJobTest, AddJobTest_Invalid_Min_RAM_TOO_SMALL_Test){
    details.config.set_min_ram(123);
    EXPECT_THROW(JobGateway::add(details), std::invalid_argument);
}

// Test to see if the addJob method throws an exception when the max_ram arg is invalid.
TEST_F(AddJobTest, AddJobTest_Invalid_Max_RAM_TOO_SMALL_Test){
    details.config.set_max_ram(123);
    EXPECT_THROW(JobGateway::add(details), std::invalid_argument);
}

// Test to see if the addJob method throws an exception when the min_ram arg is larger than the max_ram arg.
TEST_F(AddJobTest, AddJobTest_Invalid_Min_RAM_Larger_Than_Max_Test){
    details.config.set_min_ram(4194305);
    EXPECT_THROW(JobGateway::add(details), std::invalid_argument);
}

// Test to see if the addJob method throws an exception when the min_cpu_count arg is invalid.
TEST_F(AddJobTest, AddJobTest_Invalid_Invalid_Min_CPU_Count_Test){
    details.config.set_min_cpu_count(0);
    EXPECT_THROW(JobGateway::add(details), std::invalid_argument);
}

// Test to see if the addJob method throws an exception when the max_cpu_count arg is invalid.
TEST_F(AddJobTest, AddJobTest_Invalid_Max_CPU_Count_Test){
    details.config.set_max_cpu_count(0);
    EXPECT_THROW(JobGateway::add(details), std::invalid_argument);
}

// Test to see if the addJob method throws an exception when the min_cpu_count arg is larger than the max_cpu_count arg
TEST_F(AddJobTest, AddJobTest_Invalid_Min_CPU_Count_Larger_Than_Max_Test){
    details.config.set_min_cpu_count(2);
    details.config.set_max_cpu_count(1);
    EXPECT_THROW(JobGateway::add(details), std::invalid_argument);
}

/**
 * Restores the jobs table
 */
void createJobsTable(){
    QSqlQuery query("CREATE TABLE `jobs` (\n"
                    "  `id` bigint(10) unsigned NOT NULL AUTO_INCREMENT,\n"
                    "  `min_ram` bigint(10) unsigned DEFAULT NULL,\n"
                    "  `start_time` varchar(60) DEFAULT NULL,\n"
                    "  `schedule_time` varchar(60) DEFAULT NULL,\n"
                    "  `finish_time` varchar(60) DEFAULT NULL,\n"
                    "  `command` text NOT NULL,\n"
                    "  `image` varchar(500) NOT NULL,\n"
                    "  `blocking_mode` tinyint(1) DEFAULT NULL,\n"
                    "  `working_dir` text NOT NULL,\n"
                    "  `allocated_id` bigint(10) unsigned DEFAULT NULL,\n"
                    "  `interruptible` tinyint(1) DEFAULT NULL,\n"
                    "  `environment` text,\n"
                    "  `min_cores` int(10) unsigned DEFAULT NULL,\n"
                    "  `max_cores` int(10) unsigned DEFAULT NULL,\n"
                    "  `priority` int(10) unsigned DEFAULT NULL,\n"
                    "  `status_id` int(10) unsigned NOT NULL DEFAULT '1',\n"
                    "  `max_ram` bigint(10) unsigned DEFAULT NULL,\n"
                    "  `user_id` bigint(10) unsigned NOT NULL,\n"
                    "  `worker_id` bigint(10) DEFAULT NULL,\n"
                    "  `email` varchar(255) NOT NULL,\n"
                    "  `result_id` bigint(10) unsigned DEFAULT NULL,\n"
                    "  PRIMARY KEY (`id`),\n"
                    "  UNIQUE KEY `id_UNIQUE` (`id`),\n"
                    "  UNIQUE KEY `allocated_id_UNIQUE` (`allocated_id`),\n"
                    "  UNIQUE KEY `result_id` (`result_id`)\n"
                    ") ENGINE=InnoDB DEFAULT CHARSET=utf8");
    query.exec();
}

void deleteJobsTable() {
    QSqlQuery query("DROP TABLE jobs");
    query.exec();
}

/**
 * Fixture class that deletes the jobs table on setup and restores it on teardown.
 */
class NoJobsTableTest : public ::testing::Test{
protected:
    void SetUp() override {
        // Deletes the jobs table
        deleteJobsTable();

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
        createJobsTable();
    }

    job_details details;
};

// Test to see if an exception is thrown when a job is being added, but no jobs' table exists.
TEST_F(NoJobsTableTest, NoJobsTableTest_AddJob_Test){
    EXPECT_THROW(JobGateway::add(details), std::logic_error);
}

// Test to see if an exception is thrown when a job is being removed, but no jobs' table exists.
TEST_F(NoJobsTableTest, NoJobsTableTest_RemoveJob_Test){
    EXPECT_THROW(JobGateway::remove(details.id), std::logic_error);
}

// Test to see if an exception is thrown when the job getter is called, but no jobs' table exists.
TEST_F(NoJobsTableTest, NoJobsTableTest_GetJob_Test){
    EXPECT_THROW(JobGateway::getJob(details.id), std::logic_error);
}

// Test to see if an exception is thrown when the jobs getter is called, but no jobs' table exists.
TEST_F(NoJobsTableTest, NoJobsTableTest_GetJobs_Test){
    EXPECT_THROW(JobGateway::getJobs(), std::logic_error);
}

/**
 * Fixture class that initializes a sample job with only the mandatory information (a.k.a the optionals don't have
 * values)
 */
class AddJobMandatoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        details.id = 1;
        details.status = 1; //scheduled
        details.user_id = 1;
        details.command = "mkdir build";
        details.schedule_time = QDateTime::currentDateTime();
        details.empty = false;
        details.config.set_email("mail@test.com");
        details.config.set_image("testimage");
        details.config.set_current_working_dir(".");
    }

    void TearDown() override {
        resetJobTable();
    }

    job_details details;
};

// First add with only the mandatory information
TEST_F(AddJobMandatoryTest, AddJobMandatoryTest_OnlyMandatory_Test){

    EXPECT_TRUE(JobGateway::add(details) == 1);
    EXPECT_TRUE(wasJobAddSuccessful(details, 1));
}

/**
 * Check if the remove query on id was successful
 * @param id The id of the removed record.
 * @return  true if remove was successful, otherwise false.
 */
bool wasJobRemoveSuccessful(uint64_t id){
    QSqlQuery query("SELECT * FROM jobs WHERE id = ?");
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
class RemoveJobTest : public ::testing::Test {
protected:
    void TearDown() override{
        resetJobTable();
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
    details.config.set_email("mail@test.com");
    details.config.set_image("testimage");
    details.config.set_current_working_dir(".");

    // Since this is the first job, this has to be true.
    EXPECT_TRUE(JobGateway::add(details) == 1);
    EXPECT_TRUE(wasJobAddSuccessful(details, 1));

    // This must return true.
    EXPECT_TRUE(JobGateway::remove(1));
    EXPECT_TRUE(wasJobRemoveSuccessful(1));
}

// Test to see if the remove method fails when it's called with an invalid id.
TEST_F(RemoveJobTest, RemoveJobTest_FailureRemove_Test){
    EXPECT_FALSE(JobGateway::remove(1));
}

/**
 * Fixture class that initializes a sample job on setup and resets the jobs table on teardown
 */
class GetJobTest : public ::testing::Test {
protected:
    void SetUp() override {
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

void deleteAllocResTable() {
    QSqlQuery query("DROP TABLE allocated_resources");
    query.exec();

}

void createAllocResTable() {
    QSqlQuery query("CREATE TABLE IF NOT EXISTS `balancedbanana`.`allocated_resources`\n"
                  "(\n"
                  "    `id`    BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,\n"
                  "    `space` BIGINT(10) UNSIGNED NOT NULL,\n"
                  "    `ram`   BIGINT(10) UNSIGNED NOT NULL,\n"
                  "    `cores` INT(10) UNSIGNED NOT NULL,\n"
                  "    PRIMARY KEY (`id`),\n"
                  "    UNIQUE INDEX `id_UNIQUE` (`id` ASC)\n"
                  ")\n"
                  "ENGINE = InnoDB\n"
                  "DEFAULT CHARACTER SET = utf8");
    query.exec();
}

// Test to see if an exception is thrown when the getter is called but no allocated_resorces table exists
TEST_F(GetJobTest, GetJobTest_NoAllocatedResourcesTable_Test){
    // Deletes the allocated_resources table
    deleteAllocResTable();

    EXPECT_THROW(JobGateway::getJob(details.id), std::logic_error);

    // Restore the table
    createAllocResTable();
}

void deleteResultsTable() {
    QSqlQuery query("DROP TABLE job_results");
    query.exec();
}

void createResultsTable() {
    QSqlQuery query("CREATE TABLE `job_results` (\n"
                  "  `id` bigint(10) unsigned NOT NULL AUTO_INCREMENT,\n"
                  "  `stdout` text NOT NULL,\n"
                  "  `exit_code` tinyint(3) NOT NULL,\n"
                  "  PRIMARY KEY (`id`),\n"
                  "  UNIQUE KEY `id_UNIQUE` (`id`)\n"
                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8");
    query.exec();
}

// Test to see if an exception is thrown when the getter is called but no job_results table exists
TEST_F(GetJobTest, GetJobTest_NoJobResultsTable_Test){
    // Deletes the job_results table
    deleteResultsTable();

    EXPECT_THROW(JobGateway::getJob(details.id), std::logic_error);

    // Restore the table
    createResultsTable();
}

// Test to see if getter returns empty struct when no job was added
TEST_F(GetJobTest, GetJobTest_NonExistentJob_Test){
    EXPECT_TRUE(JobGateway::getJob(details.id).empty);
}

// Test to see if getter returns correct struct when a job was added
TEST_F(GetJobTest, GetJobTest_FirstAdd_Test){
    // Add the job. Should work without issues
    EXPECT_TRUE(JobGateway::add(details) == details.id);
    EXPECT_TRUE(wasJobAddSuccessful(details, details.id));
    EXPECT_TRUE(JobGateway::getJob(details.id) == details);
}

// Test to see if getter works when only mandatory job memebers were added
TEST_F(GetJobTest, GetJobTest_MandatoryAdd_Test){
    job_details details;
    details.id = 1;
    details.status = 1; //scheduled
    details.user_id = 1;
    details.command = "mkdir build";
    details.schedule_time = QDateTime::currentDateTime();
    details.empty = false;
    details.config.set_email("mail@test.com");
    details.config.set_image("testimage");
    details.config.set_current_working_dir(".");

    EXPECT_TRUE(JobGateway::add(details) == details.id);
    EXPECT_TRUE(wasJobAddSuccessful(details, details.id));
    EXPECT_TRUE(JobGateway::getJob(details.id) == details);
}

/**
 * Fixture class that initializes three jobs on setup and resets the table on teardown.
 */
class GetJobsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up the first user
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
        first.config.set_email("mail@test.com");
        first.config.set_priority(Priority::low);
        first.config.set_image("testimage");
        first.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        first.config.set_interruptible(false);
        first.config.set_current_working_dir(".");

        // Set up the second user
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
        second.config.set_email("mail@test.com");
        second.config.set_priority(Priority::high);
        second.config.set_image("testimage");
        second.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        second.config.set_interruptible(false);
        second.config.set_current_working_dir(".");

        // Set up the third user
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
        third.config.set_email("mail@test.com");
        third.config.set_priority(Priority::emergency);
        third.config.set_image("testimage");
        third.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        third.config.set_interruptible(false);
        third.config.set_current_working_dir(".");

    }

    void TearDown() override {
        resetJobTable();
    }

    job_details first;
    job_details second;
    job_details third;
};

// Test to see if getJobs retrieves a vector of previously added jobs from the database
TEST_F(GetJobsTest, GetJobsTest_SuccessfulGet_Test){
    // Add the users. Their ids should match the order of their addition.
    EXPECT_EQ(JobGateway::add(first), first.id);
    EXPECT_EQ(JobGateway::add(second), second.id);
    EXPECT_EQ(JobGateway::add(third), third.id);

    std::vector<job_details> expectedVector;
    expectedVector.push_back(first);
    expectedVector.push_back(second);
    expectedVector.push_back(third);

    std::vector<job_details> actualVector = JobGateway::getJobs();
    EXPECT_TRUE(Utilities::areDetailVectorsEqual(expectedVector, actualVector));
}

// Test to see if an exception is thrown when the getter is called but no allocated_resorces table exists
TEST_F(GetJobsTest, GetJobsTest_NoAllocatedResourcesTable_Test){
    // Deletes the allocated_resources table
    deleteAllocResTable();
    EXPECT_THROW(JobGateway::getJobs(), std::logic_error);

    // Restore the table
    createAllocResTable();
}

// Test to see if an exception is thrown when the getter is called but no job_results table exists
TEST_F(GetJobsTest, GetJobsTest_NoJobResultsTable_Test){
    // Deletes the job_results table
    deleteResultsTable();

    EXPECT_THROW(JobGateway::getJobs(), std::logic_error);

    // Restore the table
    createResultsTable();
}

// Test to see if the getter method returns an empty vector if the users table is empty
TEST_F(GetJobsTest, GetJobsTest_NonExistentJobs_Test){
    EXPECT_TRUE(JobGateway::getJobs().empty());
}

/**
 * Resets the allocated_resources table
 */
void resetAllocResTable() {
    QSqlQuery query("ALTER TABLE allocated_resources CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL");
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
class StartJobTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup Job
        job.id = 1;
        job.status = 1; //scheduled
        job.user_id = 1;
        job.command = "mkdir build";
        job.schedule_time = QDateTime::currentDateTime();
        job.empty = false;
        job.config.set_email("mail@test.com");
        job.config.set_image("testimage");
        job.config.set_current_working_dir(".");
        job.start_time = std::make_optional<QDateTime>(QDateTime::currentDateTime());

        // SetUp Worker
        worker.public_key = "sadfjsaljdf";
        worker.specs.space = 10240;
        worker.specs.ram = FOUR_MB;
        worker.specs.cores = 4;
        worker.address = "1.2.3.4";
        worker.name = "Ubuntu";
        worker.id = 1;
        worker.empty = false;
    }
    void TearDown() override {
        resetJobTable();
        resetAllocResTable();
        QSqlQuery query("ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL");
        query.exec();
        query.prepare("DELETE FROM workers");
        query.exec();
        query.prepare("ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT");
        query.exec();
    }


    job_details job;
    worker_details worker;
};

bool wasStartSuccessful(job_details job, worker_details worker){
    uint allocated_id = 1;
    QSqlQuery queryAlloc("SELECT cores, ram, space FROM allocated_resources WHERE id = ?");
    queryAlloc.addBindValue(allocated_id);
    QSqlQuery queryJobs("SELECT allocated_id, status_id, start_time FROM jobs WHERE id = ?");
    queryJobs.addBindValue(QVariant::fromValue(job.id));

    if (queryAlloc.exec()){
        if (queryAlloc.next()){
            EXPECT_EQ(queryAlloc.value(0).toUInt(), worker.specs.cores);
            EXPECT_EQ(queryAlloc.value(1).toUInt(), worker.specs.ram);
            EXPECT_EQ(queryAlloc.value(2).toUInt(), worker.specs.space);
        } else {
            return false;
        }
    } else {
        return false;
    }

    if (queryJobs.exec()){
        if (queryJobs.next()){
            EXPECT_EQ(queryJobs.value(0).toUInt(), allocated_id);
            EXPECT_EQ(queryJobs.value(1).toInt(), (int) JobStatus::processing);
            EXPECT_TRUE(QDateTime::fromString(queryJobs.value(2).toString(), "yyyy.MM.dd:hh.mm.ss.z")
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
    EXPECT_TRUE(WorkerGateway::add(worker) == worker.id);
    EXPECT_TRUE(JobGateway::add(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));

    EXPECT_TRUE(JobGateway::startJob(job.id, worker.id, worker.specs, job.start_time.value()));

    // Check if the values were set properly
    EXPECT_TRUE(wasStartSuccessful(job, worker));
}


// Test to see if exception is thrown when no workers table exists
TEST_F(StartJobTest, StartJobTest_NoWorkersTable_Test){
    QSqlQuery query("DROP TABLE workers");
    query.exec();

    EXPECT_THROW(JobGateway::startJob(job.id, worker.id, worker.specs, job.start_time.value()), std::logic_error);

    query.prepare("CREATE TABLE `workers` (`id` bigint(10) unsigned NOT NULL AUTO_INCREMENT, `ram` bigint(10) "
          "unsigned DEFAULT NULL, `cores` int(10) unsigned DEFAULT NULL,`space` bigint(10) unsigned "
          "DEFAULT NULL, `address` varchar(255) DEFAULT NULL, `public_key` varchar(255) DEFAULT NULL, "
          "`name` varchar(45) DEFAULT NULL, PRIMARY KEY (`id`), UNIQUE KEY `id_UNIQUE` (`id`), UNIQUE "
          "KEY `public_key_UNIQUE` (`public_key`), UNIQUE KEY `address_UNIQUE` (`address`) ) "
          "ENGINE=InnoDB DEFAULT CHARSET=utf8");
    query.exec();
}

// Test to see if exception is thrown when no jobs table exists
TEST_F(StartJobTest, StartJobTest_NoJobsTable_Test){
    deleteJobsTable();

    EXPECT_THROW(JobGateway::startJob(job.id, worker.id, worker.specs, job.start_time.value()), std::logic_error);

    createJobsTable();
}

// Test to see if exception is thrown when no job with the id arg exists in the database
TEST_F(StartJobTest, StartJobTest_NonExistentJob_Test){
    EXPECT_FALSE(JobGateway::startJob(job.id, worker.id, worker.specs, job.start_time.value()));
}

// Test to see if exception is thrown when no worker with the id arg exists in the database
TEST_F(StartJobTest, StartJobTest_NonExistentWorker_Test){
    EXPECT_TRUE(JobGateway::add(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    EXPECT_FALSE(JobGateway::startJob(job.id, worker.id, worker.specs, job.start_time.value()));
}

void resetResultsTable() {
        QSqlQuery query("ALTER TABLE job_results CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL");
        query.exec();
        query.prepare("DELETE FROM job_results");
        query.exec();
        query.prepare("ALTER TABLE job_results CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT");
        query.exec();
};

/**
 * Fixture class that initializes sample finish data for a Job on SetUp and resets the jobs and job_results tables on
 * TearDown
 */
class FinishJobTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup Job
        job.id = 1;
        job.status = 1; //scheduled
        job.user_id = 1;
        job.command = "mkdir build";
        job.schedule_time = QDateTime::currentDateTime();
        job.empty = false;
        job.config.set_email("mail@test.com");
        job.config.set_image("testimage");
        job.config.set_current_working_dir(".");
        job.finish_time = std::make_optional(QDateTime::currentDateTime());

        // Finish information
        stdout = "Some detailed info...";
        exit_code = 0;

    }

    void TearDown() override {
        resetJobTable();
        resetResultsTable();
    }

    job_details job;
    worker_details worker;
    std::string stdout;
    int8_t exit_code;
};

bool wasFinishSuccessful(std::string stdout, job_details job, int8_t exit_code){
    uint result_id = 1;
    QSqlQuery queryResult("SELECT stdout, exit_code FROM job_results WHERE id = ?");
    queryResult.addBindValue(result_id);
    QSqlQuery queryJobs("SELECT finish_time, result_id FROM jobs WHERE id = ?");
    queryJobs.addBindValue(QVariant::fromValue(job.id));

    if (queryResult.exec()){
        if (queryResult.next()){
            EXPECT_EQ(queryResult.value(0).toString().toStdString(), stdout);
            EXPECT_EQ(queryResult.value(1).toInt(), exit_code);
        } else {
            return false;
        }
    } else {
        ADD_FAILURE();
    }

    if (queryJobs.exec()){
        if (queryJobs.next()){
            EXPECT_TRUE(QDateTime::fromString(queryJobs.value(0).toString(), "yyyy.MM.dd:hh.mm.ss.z") == job
                    .finish_time.value());
            EXPECT_EQ(queryJobs.value(1).toUInt(), result_id);
        } else {
            return false;
        }
    } else {
        return false;
    }
}


// Test to see if successful finishJob call sets the values in all tables properly
TEST_F(FinishJobTest, FinishJobTest_SuccessfulFinish_Test){
    // Add the job to the DB. This operation should be successful
    EXPECT_TRUE(JobGateway::add(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    EXPECT_TRUE(JobGateway::finishJob(job.id, job.finish_time.value(), stdout, exit_code));

    // Check if the values were set properly
    EXPECT_TRUE(wasFinishSuccessful(stdout, job, exit_code));
}

// Test to see if exception is thrown when finishJob is called, but no job_results table exists
TEST_F(FinishJobTest, FinishJobTest_NoJobResultsTable_Test){
    deleteResultsTable();
    EXPECT_THROW(JobGateway::finishJob(job.id, job.finish_time.value(), stdout, exit_code), std::logic_error);
    createResultsTable();
}

// Test to see if exception is thrown when finishJob is called, but no jobs table exists
TEST_F(FinishJobTest, FinishJobTest_NoJobsTable_Test){
    deleteJobsTable();
    EXPECT_THROW(JobGateway::finishJob(job.id, job.finish_time.value(), stdout, exit_code), std::logic_error);
    createJobsTable();
}

// Test to see if finishJob returns false, when no job exists with the id arg
TEST_F(FinishJobTest, FinishJobTest_NonExistentJob_Test){
    EXPECT_FALSE(JobGateway::finishJob(job.id, job.finish_time.value(), stdout, exit_code));
}

// Test to see if an exception is thrown when finishJob is called with an invalid finish_time
TEST_F(FinishJobTest, FinishJobTest_InvalidFinishTime_Test){
    EXPECT_TRUE(JobGateway::add(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    job.finish_time = std::make_optional(QDateTime::fromString("0.13.54.13.01:5.5"));
    EXPECT_THROW(JobGateway::finishJob(job.id, job.finish_time.value(), stdout, exit_code), std::invalid_argument);
}

void resetWorker(){
    QSqlQuery query("ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL");
    query.exec();
    query.prepare("DELETE FROM workers");
    query.exec();
    query.prepare("ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT");
    query.exec();
}

class GetJobCompleteTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup Job
        job.id = 1;
        job.status = 1; //scheduled
        job.user_id = 1;
        job.command = "mkdir build";
        job.schedule_time = QDateTime::currentDateTime();
        job.empty = false;
        job.config.set_email("mail@test.com");
        job.config.set_image("testimage");
        job.config.set_current_working_dir(".");
        job.start_time = std::make_optional<QDateTime>(QDateTime::currentDateTime());
        job.status = (int) JobStatus::processing;
        job.worker_id = 1;

        // Finish information
        stdout = "Some detailed info...";
        exit_code = 0;


        // SetUp Worker
        worker.public_key = "sadfjsaljdf";
        worker.specs.space = 10240;
        worker.specs.ram = FOUR_MB;
        worker.specs.cores = 4;
        worker.address = "1.2.3.4";
        worker.name = "Ubuntu";
        worker.id = 1;
        worker.empty = false;

        job.allocated_specs = std::make_optional<Specs>(worker.specs);
    }

    void TearDown() override {
        resetResultsTable();
        resetJobTable();
        resetAllocResTable();
        resetWorker();
    }

    job_details job;
    worker_details worker;
    std::string stdout;
    int8_t exit_code;
};

TEST_F(GetJobCompleteTest, GetJobCompleteTest_AfterStart_Test){
    EXPECT_TRUE(WorkerGateway::add(worker) == worker.id);
    EXPECT_TRUE(JobGateway::add(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    EXPECT_TRUE(JobGateway::startJob(job.id, worker.id, worker.specs, job.start_time.value()));
    EXPECT_TRUE(wasStartSuccessful(job, worker));

    job_details queryDetails = JobGateway::getJob(job.id);
    EXPECT_TRUE(queryDetails == job);
}

TEST_F(GetJobCompleteTest, GetJobCompleteTest_AfterFinish_Test){
    EXPECT_TRUE(WorkerGateway::add(worker) == worker.id);
    EXPECT_TRUE(JobGateway::add(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    EXPECT_TRUE(JobGateway::startJob(job.id, worker.id, worker.specs, job.start_time.value()));
    EXPECT_TRUE(wasStartSuccessful(job, worker));
    job.finish_time = std::make_optional<QDateTime>(QDateTime::currentDateTime());
    EXPECT_TRUE(JobGateway::finishJob(job.id, job.finish_time.value(), stdout, exit_code));
    EXPECT_TRUE(wasFinishSuccessful(stdout, job, exit_code));

    job_details queryDetails = JobGateway::getJob(job.id);
    EXPECT_TRUE(queryDetails == job);
}