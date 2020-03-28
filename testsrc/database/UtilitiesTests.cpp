#include <gtest/gtest.h>
#include <database/Utilities.h>
#include <database/Repository.h>
#include <database/IGateway.h>
#include <QSqlQuery>

#include "DatabaseTest.h"

using namespace balancedbanana::database;

using UtilitiesTest = DatabaseTest;

using DoesTableExistTest = UtilitiesTest;

TEST_F(DoesTableExistTest, DoesTableExistTest_Jobs_Test){
    EXPECT_TRUE(Utilities::doesTableExist("jobs", db));
}


TEST_F(DoesTableExistTest, DoesTableExistTest_Users_Test){
    EXPECT_TRUE(Utilities::doesTableExist("users", db));
}


TEST_F(DoesTableExistTest, DoesTableExistTest_Workers_Test){
    EXPECT_TRUE(Utilities::doesTableExist("workers", db));
}


TEST_F(DoesTableExistTest, DoesTableExistTest_Job_Results_Test){
    EXPECT_TRUE(Utilities::doesTableExist("job_results", db));
}


TEST_F(DoesTableExistTest, DoesTableExistTest_Allocated_Resources_Test){
    EXPECT_TRUE(Utilities::doesTableExist("allocated_resources", db));
}

TEST_F(DoesTableExistTest, DoesTableExistTest_NoJobs_Test){
    QSqlQuery query("DROP TABLE jobs", db);
    query.exec();
    EXPECT_FALSE(Utilities::doesTableExist("jobs", db));
    query.prepare("CREATE TABLE IF NOT EXISTS `balancedbanana`.`jobs` (\n"
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
                  "DEFAULT CHARSET=utf8");
    query.exec();
}


TEST_F(DoesTableExistTest, DoesTableExistTest_NoUsers_Test){
    QSqlQuery query("DROP TABLE users", db);
    query.exec();
    EXPECT_FALSE(Utilities::doesTableExist("users", db));
    query.prepare("CREATE TABLE IF NOT EXISTS `balancedbanana`.`users`\n"
                  "(\n"
                  "    `id`    BIGINT(10) UNSIGNED NOT NULL,\n"
                  "    `name`  VARCHAR(45)         NOT NULL,\n"
                  "    `email` VARCHAR(255)         NOT NULL,\n"
                  "    `public_key`   LONGTEXT        NOT NULL,\n"
                  "    PRIMARY KEY (`id`),\n"
                  "    UNIQUE INDEX `id_UNIQUE` (`id` ASC),\n"
                  "    UNIQUE INDEX `name_UNIQUE` (`name` ASC)\n"
                  ")\n"
                  "ENGINE = InnoDB\n"
                  "DEFAULT CHARACTER SET = utf8");
    query.exec();
}


TEST_F(DoesTableExistTest, DoesTableExistTest_NoWorkers_Test){
    QSqlQuery query("DROP TABLE workers", db);
    query.exec();
    EXPECT_FALSE(Utilities::doesTableExist("workers", db));
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


TEST_F(DoesTableExistTest, DoesTableExistTest_NoJob_Results_Test){
    QSqlQuery query("DROP TABLE job_results", db);
    query.exec();
    EXPECT_FALSE(Utilities::doesTableExist("job_results", db));
    query.prepare("CREATE TABLE `job_results` (\n"
                    "  `id` bigint(10) unsigned NOT NULL AUTO_INCREMENT,\n"
                    "  `output` text NOT NULL,\n"
                    "  `exit_code` tinyint(3) NOT NULL,\n"
                    "  PRIMARY KEY (`id`),\n"
                    "  UNIQUE KEY `id_UNIQUE` (`id`)\n"
                    ") ENGINE=InnoDB DEFAULT CHARSET=utf8");
    query.exec();
}


TEST_F(DoesTableExistTest, DoesTableExistTest_NoAllocated_Resources_Test){
    QSqlQuery query("DROP TABLE allocated_resources", db);
    query.exec();
    EXPECT_FALSE(Utilities::doesTableExist("allocated_resources", db));
    query.prepare("CREATE TABLE IF NOT EXISTS `balancedbanana`.`allocated_resources`\n"
                  "(\n"
                  "    `id`    BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,\n"
                  "    `osIdentifier` TEXT NOT NULL,\n"
                  "    `ram`   BIGINT(10) UNSIGNED NOT NULL,\n"
                  "    `cores` INT(10) UNSIGNED NOT NULL,\n"
                  "    PRIMARY KEY (`id`),\n"
                  "    UNIQUE INDEX `id_UNIQUE` (`id` ASC)\n"
                  ")\n"
                  "ENGINE = InnoDB\n"
                  "DEFAULT CHARACTER SET = utf8");
    query.exec();
}

using AreDetailVectorsEqualTest = UtilitiesTest;

TEST_F(AreDetailVectorsEqualTest, AreDetailVectorsEqualTest_Size_Diff_Job_Test){
    job_details first;
    job_details second;
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

    std::vector<job_details> firstVector;
    firstVector.push_back(first);
    firstVector.push_back(second);

    std::vector<job_details> secondVector;
    secondVector.push_back(first);

    EXPECT_FALSE(Utilities::areDetailVectorsEqual(firstVector, secondVector));
}

TEST_F(AreDetailVectorsEqualTest, AreDetailVectorsEqualTest_Not_Equal_Jobs_Test){
    job_details first;
    job_details second;
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
    second.config.set_min_ram(4194305);
    second.config.set_max_ram(4194306);
    second.config.set_min_cpu_count(42);
    second.config.set_max_cpu_count(43);
    second.config.set_blocking_mode(true);
    second.config.set_priority(Priority::high);
    second.config.set_image("testimage");
    second.config.set_environment(std::vector<std::string>{"str1", "str2", "str3"});
    second.config.set_interruptible(false);
    second.config.set_current_working_dir(".");

    std::vector<job_details> firstVector;
    firstVector.push_back(first);

    std::vector<job_details> secondVector;
    secondVector.push_back(second);

    EXPECT_FALSE(Utilities::areDetailVectorsEqual(firstVector, secondVector));
}

TEST_F(AreDetailVectorsEqualTest, AreDetailVectorsEqualTest_Equal_Jobs_Test){
    job_details first;
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

    std::vector<job_details> firstVector;
    firstVector.push_back(first);

    std::vector<job_details> secondVector;
    secondVector.push_back(first);

    EXPECT_TRUE(Utilities::areDetailVectorsEqual(firstVector, secondVector));
}

TEST_F(AreDetailVectorsEqualTest, AreDetailVectorsEqualTest_Not_Equal_Users_Test){
    user_details first;
    user_details second;
    // Set up the first user
    first.public_key = "34nrhk3hkr";
    first.email = "someemail@kit.edu";
    first.name = "CentOS";
    first.id = 1;
    first.empty = false;

    // Set up the second user
    second.public_key = "fsd8iasdf8sadf";
    second.email = "someotheremail@kit.edu";
    second.name = "Ubuntu";
    second.id = 2;
    second.empty = false;


    std::vector<user_details> firstVector;
    firstVector.push_back(first);

    std::vector<user_details> secondVector;
    secondVector.push_back(second);

    EXPECT_FALSE(Utilities::areDetailVectorsEqual(firstVector, secondVector));
}

TEST_F(AreDetailVectorsEqualTest, AreDetailVectorsEqualTest_Equal_Users_Test){
    user_details first;
    // Set up the first user
    first.public_key = "34nrhk3hkr";
    first.email = "someemail@kit.edu";
    first.name = "CentOS";
    first.id = 1;
    first.empty = false;

    std::vector<user_details> firstVector;
    firstVector.push_back(first);

    std::vector<user_details> secondVector;
    secondVector.push_back(first);

    EXPECT_TRUE(Utilities::areDetailVectorsEqual(firstVector, secondVector));
}

TEST_F(AreDetailVectorsEqualTest, AreDetailVectorsEqualTest_Not_Equal_Workers_Test){
    worker_details first;
    worker_details second;
    // Set up the first worker
    first.public_key = "34nrhk3hkr";
    Specs firstSpecs{};
    firstSpecs.osIdentifier = "10240";
    firstSpecs.ram = 16384;
    firstSpecs.cores = 4;
    first.specs = firstSpecs;
    first.name = "CentOS";
    first.id = 1;
    first.empty = false;

    // Set up the second worker
    second.public_key = "fsd8iasdf8sadf";
    Specs secondSpecs{};
    secondSpecs.osIdentifier = "14134";
    secondSpecs.ram = 12421;
    secondSpecs.cores = 3;
    second.specs = secondSpecs;
    second.name = "Ubuntu";
    second.id = 2;
    second.empty = false;


    std::vector<worker_details> firstVector;
    firstVector.push_back(first);

    std::vector<worker_details> secondVector;
    secondVector.push_back(second);

    EXPECT_FALSE(Utilities::areDetailVectorsEqual(firstVector, secondVector));
}

TEST_F(AreDetailVectorsEqualTest, AreDetailVectorsEqualTest_Equal_Workers_Test){
    worker_details first;
    // Set up the first worker
    first.public_key = "34nrhk3hkr";
    Specs firstSpecs{};
    firstSpecs.osIdentifier = "10240";
    firstSpecs.ram = 16384;
    firstSpecs.cores = 4;
    first.specs = firstSpecs;
    first.name = "CentOS";
    first.id = 1;
    first.empty = false;


    std::vector<worker_details> firstVector;
    firstVector.push_back(first);

    std::vector<worker_details> secondVector;
    secondVector.push_back(first);

    EXPECT_TRUE(Utilities::areDetailVectorsEqual(firstVector, secondVector));
}