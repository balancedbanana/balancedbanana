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
        Repository("localhost", "balancedbanana", "balancedbanana", "qwer1234", 3306);
    }
};

::testing::Environment* const job_env = ::testing::AddGlobalTestEnvironment(new JobGatewayEnvironment);

/**
 * Deletes the all records in the jobs table and resets the auto increment for the id.
 */
void resetJobTable() {
    auto db = IGateway::AcquireDatabase();
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
        details.config.set_priority(Priority::low);
        details.config.set_image("testimage");
        details.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        details.config.set_interruptible(false);
        details.config.set_current_working_dir(".");
        details.allocated_specs->ram = 1324;
        details.allocated_specs->cores = 4;
        details.allocated_specs->osIdentifier = "55";
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
    auto db = IGateway::AcquireDatabase();
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

            queryDetails.config.set_priority(static_cast<Priority> (query.value(priority_index).toUInt()));
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
                        queryDetails.allocated_specs->osIdentifier = query.value(1).toString().toStdString();
                        queryDetails.allocated_specs->cores = query.value(2).toUInt();
                        queryDetails.allocated_specs->ram = query.value(3).toUInt();
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
TEST_F(AddJobTest, AddJobTest_FirstJobSuccess_Test){

    // The first entry's id should be 1
    EXPECT_TRUE(JobGateway::add(details) == 1);

    // The add must be successful
    //EXPECT_TRUE(wasJobAddSuccessful(details, 1));
}

// Test to see if the auto increment feature works as expected.
TEST_F(AddJobTest, AddJobTest_AddSecondJobSucess_Test){

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
    seconddetails.config.set_priority(Priority::low);
    seconddetails.config.set_image("testimage");
    seconddetails.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
    seconddetails.config.set_interruptible(false);
    seconddetails.config.set_current_working_dir(".");
    seconddetails.allocated_specs->ram = 1324;
    seconddetails.allocated_specs->cores = 4;
    seconddetails.allocated_specs->osIdentifier = "55";

    EXPECT_TRUE(JobGateway::add(seconddetails) == 2);
    EXPECT_TRUE(wasJobAddSuccessful(seconddetails, 2));
}

/**
 * Restores the jobs table
 */
void createJobsTable(){
    auto db = IGateway::AcquireDatabase();
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
                    "  `priority` int(10) unsigned NOT NULL DEFAULT '2',\n"
                    "  `status_id` int(10) unsigned NOT NULL DEFAULT '1',\n"
                    "  `max_ram` bigint(10) unsigned DEFAULT NULL,\n"
                    "  `user_id` bigint(10) unsigned NOT NULL,\n"
                    "  `worker_id` bigint(10) DEFAULT NULL,\n"
                    "  `result_id` bigint(10) unsigned DEFAULT NULL,\n"
                    "  PRIMARY KEY (`id`),\n"
                    "  UNIQUE KEY `id_UNIQUE` (`id`),\n"
                    "  UNIQUE KEY `allocated_id_UNIQUE` (`allocated_id`),\n"
                    "  UNIQUE KEY `result_id` (`result_id`)\n"
                    ") ENGINE=InnoDB DEFAULT CHARSET=utf8", db);
    query.exec();
}

void deleteJobsTable() {
    auto db = IGateway::AcquireDatabase();
    QSqlQuery query("DROP TABLE jobs", db);
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
        details.config.set_priority(Priority::low);
        details.config.set_image("testimage");
        details.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        details.config.set_interruptible(false);
        details.config.set_current_working_dir(".");
        details.allocated_specs->ram = 1324;
        details.allocated_specs->cores = 4;
        details.allocated_specs->osIdentifier = "55";
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
        details.config.set_image("testimage");
        details.config.set_current_working_dir(".");
        details.config.set_priority(Priority::high);
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
    auto db = IGateway::AcquireDatabase();
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
    details.config.set_image("testimage");
    details.config.set_current_working_dir(".");
    details.config.set_priority(Priority::low);

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
        details.config.set_priority(Priority::low);
        details.config.set_image("testimage");
        details.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        details.config.set_interruptible(false);
        details.config.set_current_working_dir(".");
        details.allocated_specs->ram = 1324;
        details.allocated_specs->cores = 4;
        details.allocated_specs->osIdentifier = "55";
    }

    void TearDown() override {
        resetJobTable();
    }

    job_details details;
};

void deleteAllocResTable() {
    auto db = IGateway::AcquireDatabase();
    QSqlQuery query("DROP TABLE allocated_resources", db);
    query.exec();

}

void createAllocResTable() {
    auto db = IGateway::AcquireDatabase();
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
    deleteAllocResTable();

    EXPECT_THROW(JobGateway::getJob(details.id), std::logic_error);

    // Restore the table
    createAllocResTable();
}

void deleteResultsTable() {
    auto db = IGateway::AcquireDatabase();
    QSqlQuery query("DROP TABLE job_results", db);
    query.exec();
}

void createResultsTable() {
    auto db = IGateway::AcquireDatabase();
    QSqlQuery query("CREATE TABLE `job_results` (\n"
                  "  `id` bigint(10) unsigned NOT NULL AUTO_INCREMENT,\n"
                  "  `stdout` text NOT NULL,\n"
                  "  `exit_code` tinyint(3) NOT NULL,\n"
                  "  PRIMARY KEY (`id`),\n"
                  "  UNIQUE KEY `id_UNIQUE` (`id`)\n"
                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8", db);
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
    details.config.set_image("testimage");
    details.config.set_current_working_dir(".");
    details.config.set_priority(Priority::low);

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
    // Add the jobs. Their ids should match the order of their addition.
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

// Test to see if the getter method returns an empty vector if the jobs table is empty
TEST_F(GetJobsTest, GetJobsTest_NonExistentJobs_Test){
    EXPECT_TRUE(JobGateway::getJobs().empty());
}

/**
 * Resets the allocated_resources table
 */
void resetAllocResTable() {
    auto db = IGateway::AcquireDatabase();
    QSqlQuery query("ALTER TABLE allocated_resources CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL",
                    db);
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
        worker.address = "1.2.3.4";
        worker.name = "Ubuntu";
        worker.id = 1;
        worker.empty = false;
    }
    void TearDown() override {
        resetJobTable();
        resetAllocResTable();
        auto db = IGateway::AcquireDatabase();
        QSqlQuery query("ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL",
                        db);
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
    auto db = IGateway::AcquireDatabase();
    QSqlQuery queryAlloc("SELECT cores, ram, osIdentifier FROM allocated_resources WHERE id = ?",
            db);
    queryAlloc.addBindValue(allocated_id);
    auto dbb = IGateway::AcquireDatabase();
    QSqlQuery queryJobs("SELECT allocated_id, status_id, start_time FROM jobs WHERE id = ?", dbb);
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
            EXPECT_EQ(queryJobs.value(1).toInt(), (int) JobStatus::processing);
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
    EXPECT_TRUE(WorkerGateway::add(worker) == worker.id);
    EXPECT_TRUE(JobGateway::add(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));

    job.start_time = std::make_optional<QDateTime>(QDateTime::currentDateTime());
    EXPECT_TRUE(JobGateway::startJob(job.id, worker.id, worker.specs.value(), job.start_time.value()));


    // Check if the values were set properly
    EXPECT_TRUE(wasStartSuccessful(job, worker));
}


// Test to see if exception is thrown when no workers table exists
TEST_F(StartJobTest, StartJobTest_NoWorkersTable_Test){
    auto db = IGateway::AcquireDatabase();
    QSqlQuery query("DROP TABLE workers", db);
    query.exec();

    job.start_time = QDateTime::currentDateTime();
    EXPECT_THROW(JobGateway::startJob(job.id, worker.id, worker.specs.value(), job.start_time.value()), std::logic_error);
    query.prepare("CREATE TABLE IF NOT EXISTS `balancedbanana`.`workers`\n"
                  "(\n"
                  "    `id`         BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,\n"
                  "    `ram`        BIGINT(10) UNSIGNED NULL DEFAULT NULL,\n"
                  "    `cores`      INT(10) UNSIGNED NULL DEFAULT NULL,\n"
                  "    `osIdentifier`   TEXT NULL DEFAULT NULL,\n"
                  "    `address`    VARCHAR(255)        NULL DEFAULT NULL,\n"
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
    deleteJobsTable();
    job.start_time = QDateTime::currentDateTime();
    EXPECT_THROW(JobGateway::startJob(job.id, worker.id, worker.specs.value(), job.start_time.value()), std::logic_error);

    createJobsTable();
}

// Test to see if exception is thrown when no job with the id arg exists in the database
TEST_F(StartJobTest, StartJobTest_NonExistentJob_Test){
    job.start_time = QDateTime::currentDateTime();
    EXPECT_FALSE(JobGateway::startJob(job.id, worker.id, worker.specs.value(), job.start_time.value()));
}

// Test to see if exception is thrown when no worker with the id arg exists in the database
TEST_F(StartJobTest, StartJobTest_NonExistentWorker_Test){
    EXPECT_TRUE(JobGateway::add(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    job.start_time = QDateTime::currentDateTime();
    EXPECT_FALSE(JobGateway::startJob(job.id, worker.id, worker.specs.value(), job.start_time.value()));
}

void resetResultsTable() {
    auto db = IGateway::AcquireDatabase();
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
        job.config.set_image("testimage");
        job.config.set_current_working_dir(".");
        job.config.set_priority(Priority::emergency);

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
    auto db = IGateway::AcquireDatabase();
    QSqlQuery queryResult("SELECT stdout, exit_code FROM job_results WHERE id = ?", db);
    queryResult.addBindValue(result_id);
    db = IGateway::AcquireDatabase();
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
    EXPECT_TRUE(JobGateway::add(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    job.finish_time = std::make_optional(QDateTime::currentDateTime());
    EXPECT_TRUE(JobGateway::finishJob(job.id, job.finish_time.value(), stdout, exit_code));

    // Check if the values were set properly
    EXPECT_TRUE(wasFinishSuccessful(stdout, job, exit_code));
}

// Test to see if exception is thrown when finishJob is called, but no job_results table exists
TEST_F(FinishJobTest, FinishJobTest_NoJobResultsTable_Test){
    deleteResultsTable();
    job.finish_time = std::make_optional(QDateTime::currentDateTime());
    EXPECT_THROW(JobGateway::finishJob(job.id, job.finish_time.value(), stdout, exit_code), std::logic_error);
    createResultsTable();
}

// Test to see if exception is thrown when finishJob is called, but no jobs table exists
TEST_F(FinishJobTest, FinishJobTest_NoJobsTable_Test){
    deleteJobsTable();
    job.finish_time = std::make_optional(QDateTime::currentDateTime());
    EXPECT_THROW(JobGateway::finishJob(job.id, job.finish_time.value(), stdout, exit_code), std::logic_error);
    createJobsTable();
}

// Test to see if finishJob returns false, when no job exists with the id arg
TEST_F(FinishJobTest, FinishJobTest_NonExistentJob_Test){
    job.finish_time = QDateTime::currentDateTime();
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
    auto db = IGateway::AcquireDatabase();
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
class GetJobCompleteTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup Job
        job.id = 1;
        job.user_id = 1;
        job.command = "mkdir build";
        job.schedule_time = QDateTime::currentDateTime();
        job.empty = false;
        job.config.set_image("testimage");
        job.config.set_current_working_dir(".");
        job.status = (int)JobStatus::scheduled;
        job.start_time = std::nullopt;
        job.allocated_specs = std::nullopt;
        job.finish_time = std::nullopt;
        job.worker_id = std::nullopt;
        job.config.set_priority(Priority::high);

        // Finish information
        job_result result;
        result.stdout = "Some detailed info...";
        result.exit_code = 0;


        // SetUp Worker
        worker.public_key = "sadfjsaljdf";
        Specs specs{};
        specs.osIdentifier = "10240";
        specs.ram = FOUR_MB;
        specs.cores = 4;
        worker.specs = specs;
        worker.address = "1.2.3.4";
        worker.name = "Ubuntu";
        worker.id = 1;
        worker.empty = false;
    }

    void TearDown() override {
        resetResultsTable();
        resetJobTable();
        resetAllocResTable();
        resetWorker();
    }

    job_details job;
    worker_details worker;
    job_result result;
    std::string stdout;
    int8_t exit_code;
};

// Getter test for after startJob is called
TEST_F(GetJobCompleteTest, GetJobCompleteTest_AfterStart_Test){
    EXPECT_TRUE(WorkerGateway::add(worker) == worker.id);
    EXPECT_TRUE(JobGateway::add(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    job.start_time = QDateTime::currentDateTime();
    job.worker_id = 1;
    job.allocated_specs = worker.specs;
    job.status = (int) JobStatus::processing;
    EXPECT_TRUE(JobGateway::startJob(job.id, worker.id, worker.specs.value(), job.start_time.value()));
    EXPECT_TRUE(wasStartSuccessful(job, worker));

    job_details queryDetails = JobGateway::getJob(job.id);
    EXPECT_TRUE(queryDetails == job);
}

// Getter test for after finishJob is called
TEST_F(GetJobCompleteTest, GetJobCompleteTest_AfterFinish_Test){
    EXPECT_TRUE(WorkerGateway::add(worker) == worker.id);
    EXPECT_TRUE(JobGateway::add(job) == job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    job.start_time = QDateTime::currentDateTime();
    job.worker_id = 1;
    job.allocated_specs = worker.specs;
    job.status = (int) JobStatus::processing;
    EXPECT_TRUE(JobGateway::startJob(job.id, worker.id, worker.specs.value(), job.start_time.value()));
    EXPECT_TRUE(wasStartSuccessful(job, worker));
    job.finish_time = QDateTime::currentDateTime().addDays(2);
    job.result = result;
    job.status = (int) JobStatus::finished;
    EXPECT_TRUE(JobGateway::finishJob(job.id, job.finish_time.value(), result.stdout, result.exit_code));
    EXPECT_TRUE(wasFinishSuccessful(result.stdout, job, result.exit_code));

    job_details queryDetails = JobGateway::getJob(job.id);
    EXPECT_TRUE(queryDetails == job);
}

/**
 * Fixture class that initializes three jobs on setup and resets the tables on teardown.
 */
class GetJobsWithWorkerIdTest : public ::testing::Test {
protected:
    void SetUp() override {
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
        worker.address = "1.2.3.4";
        worker.name = "Ubuntu";
        worker.id = 1;
        worker.empty = false;

    }

    void TearDown() override {
        resetJobTable();
        resetWorker();
        resetAllocResTable();
    }

    job_details first;
    job_details second;
    job_details third;
    worker_details worker;
};

// Test to see if getJobsWithWorkerId retrieves a vector of previously added jobs from the database.
// second and third are started, so the method only include them in the returned vector even if other jobs exist in
// the database
TEST_F(GetJobsWithWorkerIdTest, GetJobsWithWorkerIdTest_SuccessfulGet_Test){
    // Add the jobs. Their ids should match the order of their addition.
    EXPECT_EQ(JobGateway::add(first), first.id);
    EXPECT_EQ(JobGateway::add(second), second.id);
    EXPECT_EQ(JobGateway::add(third), third.id);
    EXPECT_EQ(WorkerGateway::add(worker), worker.id);
    EXPECT_TRUE(JobGateway::startJob(second.id, worker.id, worker.specs.value(),
            QDateTime::fromString("2020.02.20:13.13.13.5", TIME_FORMAT)));
    EXPECT_TRUE(JobGateway::startJob(third.id, worker.id, worker.specs.value(),
                                     QDateTime::fromString("2020.02.21:13.13.13.5", TIME_FORMAT)));
    second.status = (int) JobStatus::processing;
    third.status = (int) JobStatus::processing;
    second.allocated_specs = worker.specs;
    third.allocated_specs = worker.specs;


    std::vector<job_details> expectedVector;
    expectedVector.push_back(second);
    expectedVector.push_back(third);

    std::vector<job_details> actualVector = JobGateway::getJobsWithWorkerId(third.worker_id.value());
    EXPECT_TRUE(Utilities::areDetailVectorsEqual(expectedVector, actualVector));
}

// Test to see if an exception is thrown when the getter is called but no allocated_resources table exists
TEST_F(GetJobsWithWorkerIdTest, GetJobsWithWorkerIdTest_NoAllocatedResourcesTable_Test){
    // Deletes the allocated_resources table
    deleteAllocResTable();
    EXPECT_THROW(JobGateway::getJobsWithWorkerId(worker.id), std::logic_error);

    // Restore the table
    createAllocResTable();
}

// Test to see if an exception is thrown when the getter is called but no job_results table exists
TEST_F(GetJobsWithWorkerIdTest, GetJobsWithWorkerIdTest_NoJobResultsTable_Test){
    // Deletes the job_results table
    deleteResultsTable();

    EXPECT_THROW(JobGateway::getJobsWithWorkerId(worker.id), std::logic_error);

    // Restore the table
    createResultsTable();
}

// Test to see if the getter method returns an empty vector if the jobs table is empty
TEST_F(GetJobsWithWorkerIdTest, GetJobsWithWorkerIdTest_NonExistentJobs_Test){
    EXPECT_TRUE(JobGateway::getJobsWithWorkerId(worker.id).empty());
}

class GetJobsInIntervalTest : public ::testing::Test {
protected:
    void SetUp() override{
        // Set up the first job
        first.id = 1;
        first.user_id = 3;
        first.command = "mkdir build";
        first.status = (int) JobStatus::scheduled;
        first.schedule_time = QDateTime::currentDateTime().addDays(-7);
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
        second.schedule_time = QDateTime::currentDateTime().addDays(-8);
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
        third.schedule_time = QDateTime::currentDateTime().addDays(-2);
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

        // Standard procedure to add jobs and to get them. This should work flawlessly
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

    void TearDown() override {
        resetJobTable();
        resetWorker();
        resetAllocResTable();
        resetResultsTable();
    }

    job_details first;
    job_details second;
    job_details third;
};

/**
 * Test to see if getJobsInInterval gets the correct scheduled jobs.
 *
 * In this case, the test attempts to retrieve the jobs that were scheduled in the last week. The first job was
 * scheduled one week prior, the second job 8 days prior and the third job 2 days prior.
 */
TEST_F(GetJobsInIntervalTest, GetJobsInIntervalTest_Scheduled_Test){
    std::vector<job_details> actualIntervalJobs = JobGateway::getJobsInInterval(QDateTime::currentDateTime().addDays
            (-7),QDateTime::currentDateTime(), JobStatus::scheduled);

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
    worker.address = "1.2.3.4";
    worker.name = "Ubuntu";
    worker.id = 1;
    worker.empty = false;
    EXPECT_EQ(WorkerGateway::add(worker), worker.id);
    worker.public_key = "asdfsadcsadcsa";
    worker.address = "6.4.23.2";
    EXPECT_EQ(WorkerGateway::add(worker), worker.id + 1);
    // The job then have to be started.
    first.start_time = QDateTime::currentDateTime().addDays(-1);
    first.status = (int) JobStatus::processing;
    first.allocated_specs = worker.specs;
    first.worker_id = worker.id;
    EXPECT_TRUE(JobGateway::startJob(first.id, worker.id, worker.specs.value(), first.start_time.value()));
    EXPECT_TRUE(JobGateway::startJob(second.id, worker.id + 1, worker.specs.value(), QDateTime::currentDateTime().addDays
    (-3)));

    // This is where the test begins.
    std::vector<job_details> actualIntervalJobs = JobGateway::getJobsInInterval(QDateTime::currentDateTime().addDays
            (-2), QDateTime::currentDateTime(), JobStatus::processing);
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
    worker.address = "1.2.3.4";
    worker.name = "Ubuntu";
    worker.id = 1;
    worker.empty = false;
    EXPECT_EQ(WorkerGateway::add(worker), worker.id);
    worker.public_key = "asdfsadcsadcsa";
    worker.address = "6.4.23.2";
    EXPECT_EQ(WorkerGateway::add(worker), worker.id + 1);
    // The job then have to be started.
    first.start_time = QDateTime::currentDateTime().addDays(-1);
    first.status = (int) JobStatus::processing;
    first.allocated_specs = worker.specs;
    first.worker_id = worker.id;
    EXPECT_TRUE(JobGateway::startJob(first.id, worker.id, worker.specs.value(), first.start_time.value()));
    EXPECT_TRUE(JobGateway::startJob(second.id, worker.id + 1, worker.specs.value(), QDateTime::currentDateTime().addDays
    (-3)));

    // Finish the third job
    third.finish_time = QDateTime::currentDateTime().addDays(-1);
    third.status = (int)JobStatus::finished;
    job_result result;
    result.stdout = "error";
    result.exit_code = 0;
    third.result = result;
    EXPECT_TRUE(JobGateway::finishJob(third.id, third.finish_time.value(), third.result->stdout, third
    .result->exit_code));
     std::vector<job_details> actualIntervalJobs = JobGateway::getJobsInInterval(QDateTime::currentDateTime().addDays
             (-1), QDateTime::currentDateTime(), JobStatus::finished);
    std::vector<job_details> expectedIntervalJobs;
    expectedIntervalJobs.push_back(third);
    EXPECT_TRUE(Utilities::areDetailVectorsEqual(expectedIntervalJobs, actualIntervalJobs));
}

// Test to see if method returns empty vector when status is not either processing, finished or scheduled.
TEST_F(GetJobsInIntervalTest, GetJobsInIntervalTest_InvalidStatus_Test){
    EXPECT_TRUE(JobGateway::getJobsInInterval(QDateTime::currentDateTime(), QDateTime::currentDateTime(),
            JobStatus::interrupted).empty());
}

// Test to see if exception is thrown when lower bound of interval is greater than upper bound.
TEST_F(GetJobsInIntervalTest, GetJobsInIntervalTest_InvalidInterval_Test){
    EXPECT_THROW(JobGateway::getJobsInInterval(QDateTime::currentDateTime().addDays(1), QDateTime::currentDateTime(),
            JobStatus::processing), std::invalid_argument);
}

class UpdateJobTest : public ::testing::Test {
protected:
    void SetUp() override {
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
        worker.address = "1.2.3.4";
        worker.name = "Ubuntu";
        worker.id = 1;
        worker.empty = false;
    }

    void TearDown() override {
        resetJobTable();
        resetAllocResTable();
        resetWorker();
    }

    job_details job;
    worker_details worker;
};

TEST_F(UpdateJobTest, UpdateJobTest_NoJobsTable_Test){
    deleteJobsTable();
    EXPECT_THROW(JobGateway::updateJob(job), std::logic_error);
    createJobsTable();
}

TEST_F(UpdateJobTest, UpdateJobTest_NoAllocResourcesTable_Test){
    deleteAllocResTable();
    EXPECT_THROW(JobGateway::updateJob(job), std::logic_error);
    createAllocResTable();
}

TEST_F(UpdateJobTest, UpdateJobTest_NoResultsTable_Test){
    deleteResultsTable();
    EXPECT_THROW(JobGateway::updateJob(job), std::logic_error);
    createResultsTable();
}

TEST_F(UpdateJobTest, UpdateJobTest_NoWorkersTable_Test){
    auto db = IGateway::AcquireDatabase();
    QSqlQuery query("DROP TABLE workers", db);
    query.exec();
    EXPECT_THROW(JobGateway::updateJob(job), std::logic_error);
    query.prepare("CREATE TABLE IF NOT EXISTS `balancedbanana`.`workers`\n"
                  "(\n"
                  "    `id`         BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,\n"
                  "    `ram`        BIGINT(10) UNSIGNED NULL DEFAULT NULL,\n"
                  "    `cores`      INT(10) UNSIGNED NULL DEFAULT NULL,\n"
                  "    `osIdentifier`   TEXT NULL DEFAULT NULL,\n"
                  "    `address`    VARCHAR(255)        NULL DEFAULT NULL,\n"
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
    EXPECT_THROW(JobGateway::updateJob(job), std::invalid_argument);
}

TEST_F(UpdateJobTest, UpdateJobTest_NonExistentJob_Test){
    EXPECT_THROW(JobGateway::updateJob(job), std::runtime_error);
}

TEST_F(UpdateJobTest, UpdateJobTest_UpdateAllocRes_Success_Test){
    // Add the job and worker and then start the job.
    EXPECT_EQ(JobGateway::add(job), job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    EXPECT_EQ(WorkerGateway::add(worker), worker.id);
    job.status = (int) JobStatus::processing;
    job.start_time = QDateTime::currentDateTime();
    job.allocated_specs = worker.specs;
    job.worker_id = 1;
    EXPECT_TRUE(JobGateway::startJob(job.id, worker.id, worker.specs.value(), job.start_time.value()));
    EXPECT_TRUE(wasStartSuccessful(job, worker));

    // Change the allocated resources
    Specs new_specs = worker.specs.value();
    new_specs.cores = worker.specs->cores + 2;
    job.allocated_specs = new_specs;
    JobGateway::updateJob(job);

    auto db = IGateway::AcquireDatabase();
    QSqlQuery query("SELECT allocated_id FROM jobs WHERE id = ?", db);
    query.addBindValue(QVariant::fromValue(job.id));
    query.exec();
    query.next();
    db = IGateway::AcquireDatabase();
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
    EXPECT_EQ(JobGateway::add(job), job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    JobGateway::updateJob(job);

    // Nothing changed
    job_details actualJob = JobGateway::getJob(job.id);
    EXPECT_TRUE(job == actualJob);
}

TEST_F(UpdateJobTest, UpdateJobTest_UpdateWorkerId_Test){
    // Add the job and  two workers and then start the job.
    EXPECT_EQ(JobGateway::add(job), job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    EXPECT_EQ(WorkerGateway::add(worker), worker.id);
    worker.public_key = "sadfsdcasd";
    worker.address = "1.2.4.5.6";
    EXPECT_EQ(WorkerGateway::add(worker), worker.id + 1);
    job.status = (int) JobStatus::processing;
    job.start_time = QDateTime::currentDateTime();
    job.allocated_specs = worker.specs;
    job.worker_id = 1;
    EXPECT_TRUE(JobGateway::startJob(job.id, worker.id, worker.specs.value(), job.start_time.value()));
    EXPECT_TRUE(wasStartSuccessful(job, worker));

    // Update the worker_id
    job.worker_id = worker.id + 1;
    JobGateway::updateJob(job);
    auto db = IGateway::AcquireDatabase();
    QSqlQuery query("SELECT worker_id FROM jobs WHERE id = ?", db);
    query.addBindValue(QVariant::fromValue(job.id));
    query.exec();
    query.next();
    EXPECT_EQ(query.value(0).toUInt(), job.worker_id);
}

TEST_F(UpdateJobTest, UpdateJobTest_UpdateStatusInterrupted_Test){
    EXPECT_EQ(JobGateway::add(job), job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    job.status = (int) JobStatus::interrupted;
    JobGateway::updateJob(job);

    job_details actualJob = JobGateway::getJob(job.id);
    EXPECT_TRUE(job == actualJob);
}

TEST_F(UpdateJobTest, UpdateJobTest_UpdateStatusPaused_Test){
    EXPECT_EQ(JobGateway::add(job), job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    job.status = (int) JobStatus::paused;
    JobGateway::updateJob(job);

    job_details actualJob = JobGateway::getJob(job.id);
    EXPECT_TRUE(job == actualJob);
}

TEST_F(UpdateJobTest, UpdateJobTest_UpdateStatusCanceled_Test){
    EXPECT_EQ(JobGateway::add(job), job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    job.status = (int) JobStatus::canceled;
    JobGateway::updateJob(job);

    job_details actualJob = JobGateway::getJob(job.id);
    EXPECT_TRUE(job == actualJob);
}

TEST_F(UpdateJobTest, UpdateJobTest_UpdatePriority_Test){
    EXPECT_EQ(JobGateway::add(job), job.id);
    EXPECT_TRUE(wasJobAddSuccessful(job, job.id));
    job.config.set_priority(Priority::emergency);
    JobGateway::updateJob(job);

    job_details actualJob = JobGateway::getJob(job.id);
    EXPECT_TRUE(job == actualJob);
}