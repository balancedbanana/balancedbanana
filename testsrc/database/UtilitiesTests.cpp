#include <gtest/gtest.h>
#include <database/Utilities.h>
#include <database/Repository.h>
#include <QSqlQuery>

using namespace balancedbanana::database;


class UtilitiesEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        Repository("localhost", "balancedbanana", "balancedbanana", "qwer1234", 3306);
    }
};

::testing::Environment* const util_env = ::testing::AddGlobalTestEnvironment(new UtilitiesEnvironment);


TEST(DoesTableExistTest, DoesTableExistTest_Jobs_Test){
    EXPECT_TRUE(Utilities::doesTableExist("jobs"));
}


TEST(DoesTableExistTest, DoesTableExistTest_Users_Test){
    EXPECT_TRUE(Utilities::doesTableExist("users"));
}


TEST(DoesTableExistTest, DoesTableExistTest_Workers_Test){
    EXPECT_TRUE(Utilities::doesTableExist("workers"));
}


TEST(DoesTableExistTest, DoesTableExistTest_Job_Results_Test){
    EXPECT_TRUE(Utilities::doesTableExist("job_results"));
}


TEST(DoesTableExistTest, DoesTableExistTest_Allocated_Resources_Test){
    EXPECT_TRUE(Utilities::doesTableExist("allocated_resources"));
}

TEST(DoesTableExistTest, DoesTableExistTest_NoJobs_Test){
    QSqlQuery query("DROP TABLE jobs");
    query.exec();
    EXPECT_FALSE(Utilities::doesTableExist("jobs"));
    query.prepare("CREATE TABLE `jobs` (\n"
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
                    "  `result_id` bigint(10) unsigned DEFAULT NULL,\n"
                    "  PRIMARY KEY (`id`),\n"
                    "  UNIQUE KEY `id_UNIQUE` (`id`),\n"
                    "  UNIQUE KEY `allocated_id_UNIQUE` (`allocated_id`),\n"
                    "  UNIQUE KEY `result_id` (`result_id`)\n"
                    ") ENGINE=InnoDB DEFAULT CHARSET=utf8");
    query.exec();
}


TEST(DoesTableExistTest, DoesTableExistTest_NoUsers_Test){
    QSqlQuery query("DROP TABLE users");
    query.exec();
    EXPECT_FALSE(Utilities::doesTableExist("users"));
    query.prepare("CREATE TABLE `users` (\n"
                    "  `name` varchar(45) NOT NULL,\n"
                    "  `email` varchar(255) NOT NULL,\n"
                    "  `public_key` varchar(255) NOT NULL,\n"
                    "  `id` bigint(10) unsigned NOT NULL AUTO_INCREMENT,\n"
                    "  PRIMARY KEY (`id`),\n"
                    "  UNIQUE KEY `public_key_UNIQUE` (`public_key`),\n"
                    "  UNIQUE KEY `id_UNIQUE` (`id`)\n"
                    ") ENGINE=InnoDB DEFAULT CHARSET=utf8");
    query.exec();
}


TEST(DoesTableExistTest, DoesTableExistTest_NoWorkers_Test){
    QSqlQuery query("DROP TABLE workers");
    query.exec();
    EXPECT_FALSE(Utilities::doesTableExist("workers"));
    query.prepare("CREATE TABLE `workers` (`id` bigint(10) unsigned NOT NULL AUTO_INCREMENT, `ram` bigint(10) "
                    "unsigned DEFAULT NULL, `cores` int(10) unsigned DEFAULT NULL,`space` bigint(10) unsigned "
                    "DEFAULT NULL, `address` varchar(255) DEFAULT NULL, `public_key` varchar(255) DEFAULT NULL, "
                    "`name` varchar(45) DEFAULT NULL, PRIMARY KEY (`id`), UNIQUE KEY `id_UNIQUE` (`id`), UNIQUE "
                    "KEY `public_key_UNIQUE` (`public_key`), UNIQUE KEY `address_UNIQUE` (`address`) ) "
                    "ENGINE=InnoDB DEFAULT CHARSET=utf8");
    query.exec();
}


TEST(DoesTableExistTest, DoesTableExistTest_NoJob_Results_Test){
    QSqlQuery query("DROP TABLE job_results");
    query.exec();
    EXPECT_FALSE(Utilities::doesTableExist("job_results"));
    query.prepare("CREATE TABLE `job_results` (\n"
                    "  `id` bigint(10) unsigned NOT NULL AUTO_INCREMENT,\n"
                    "  `stdout` text NOT NULL,\n"
                    "  `exit_code` tinyint(3) NOT NULL,\n"
                    "  PRIMARY KEY (`id`),\n"
                    "  UNIQUE KEY `id_UNIQUE` (`id`)\n"
                    ") ENGINE=InnoDB DEFAULT CHARSET=utf8");
    query.exec();
}


TEST(DoesTableExistTest, DoesTableExistTest_NoAllocated_Resources_Test){
    QSqlQuery query("DROP TABLE allocated_resources");
    query.exec();
    EXPECT_FALSE(Utilities::doesTableExist("allocated_resources"));
    query.prepare("CREATE TABLE IF NOT EXISTS `balancedbanana`.`allocated_resources`\n"
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

TEST(AreDetailVectorsEqualTest, AreDetailVectorsEqualTest_Size_Diff_Job_Test){
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

TEST(AreDetailVectorsEqualTest, AreDetailVectorsEqualTest_Not_Equal_Jobs_Test){
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

TEST(AreDetailVectorsEqualTest, AreDetailVectorsEqualTest_Equal_Jobs_Test){
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

TEST(AreDetailVectorsEqualTest, AreDetailVectorsEqualTest_Not_Equal_Users_Test){
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

TEST(AreDetailVectorsEqualTest, AreDetailVectorsEqualTest_Equal_Users_Test){
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

TEST(AreDetailVectorsEqualTest, AreDetailVectorsEqualTest_Not_Equal_Workers_Test){
    worker_details first;
    worker_details second;
    // Set up the first worker
    first.public_key = "34nrhk3hkr";
    first.specs.space = 10240;
    first.specs.ram = 16384;
    first.specs.cores = 4;
    first.address = "0.0.0.0";
    first.name = "CentOS";
    first.id = 1;
    first.empty = false;

    // Set up the second worker
    second.public_key = "fsd8iasdf8sadf";
    second.specs.space = 14134;
    second.specs.ram = 12421;
    second.specs.cores = 3;
    second.address = "1.1.1.1";
    second.name = "Ubuntu";
    second.id = 2;
    second.empty = false;


    std::vector<worker_details> firstVector;
    firstVector.push_back(first);

    std::vector<worker_details> secondVector;
    secondVector.push_back(second);

    EXPECT_FALSE(Utilities::areDetailVectorsEqual(firstVector, secondVector));
}

TEST(AreDetailVectorsEqualTest, AreDetailVectorsEqualTest_Equal_Workers_Test){
    worker_details first;
    // Set up the first worker
    first.public_key = "34nrhk3hkr";
    first.specs.space = 10240;
    first.specs.ram = 16384;
    first.specs.cores = 4;
    first.address = "0.0.0.0";
    first.name = "CentOS";
    first.id = 1;
    first.empty = false;


    std::vector<worker_details> firstVector;
    firstVector.push_back(first);

    std::vector<worker_details> secondVector;
    secondVector.push_back(first);

    EXPECT_TRUE(Utilities::areDetailVectorsEqual(firstVector, secondVector));
}