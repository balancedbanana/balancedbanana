#include <gtest/gtest.h>
#include <database/WorkerGateway.h>
#include <database/worker_details.h>
#include <database/Repository.h>
#include <database/Utilities.h>

#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <random>
#include <QTextCodec>
#include <iostream>
#include <string>
#include <locale>

using namespace balancedbanana::database;

/**
 * Test environment that has a global SetUp and TearDown methods for all test suites/tests.
 *
 * Sets up the database connection.
 *
 */
class WorkerGatewayEnvironment : public ::testing::Environment{
public:
    void SetUp() override {
        Repository("localhost", "balancedbanana", "balancedbanana", "qwer1234", 3306);
    }
};

::testing::Environment* const worker_env = ::testing::AddGlobalTestEnvironment(new WorkerGatewayEnvironment);

/**
 * Test to see if the connection to the DB was correctly established.
 */
TEST(ConnectionTest, ConnectionTest_CheckkDBConnection_Test){}

/**
 * Deletes the all records in the workers table and resets the auto increment for the id.
 */
void resetWorkerTable(){
    auto db = IGateway::AcquireDatabase();
    QSqlQuery query("ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL", db);
    query.exec();
    query.prepare("DELETE FROM workers");
    query.exec();
    query.prepare("ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT");
    query.exec();
}

/**
 * Fixture class that initializes a sample worker's details.
 */
class AddWorkerTest : public ::testing::Test {
protected:
    void SetUp() override {
        details.public_key = "34nrhk3hkr";
        Specs specs{};
        specs.osIdentifier = "Windows 10.4.1.4";
        specs.cores = 4;
        specs.ram = 16234;
        details.specs = specs;
        details.address = "0.0.0.0";
        details.name = "CentOS";
        details.empty = false;
        details.id = 1;
    }

    void TearDown() override {
        resetWorkerTable();
    }

    worker_details details;
};

/**
 * Checks if the add query was successful.
 * @param details The record details that were added with the query.
 * @param id The id of the added record.
 * @return true if the add was successful, otherwise false.
 */
bool wasWorkerAddSuccessful(const worker_details& details, uint64_t id){
    QSqlQuery query("SELECT * FROM workers WHERE id = ?", IGateway::AcquireDatabase());
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        if (query.next()){
            int nameIndex = query.record().indexOf("name");
            int ramIndex = query.record().indexOf("ram");
            int coresIndex = query.record().indexOf("cores");
            int osIndex = query.record().indexOf("osIdentifier");
            int addressIndex = query.record().indexOf("address");
            int keyIndex = query.record().indexOf("public_key");

            worker_details queryDetails{};
            queryDetails.name = query.value(nameIndex).toString().toStdString();
            if (query.value(coresIndex).isNull()){
                queryDetails.specs = std::nullopt;
            } else {
                Specs specs{};
                specs.cores = query.value(coresIndex).toUInt();
                specs.ram = query.value(ramIndex).toUInt();
                specs.osIdentifier = query.value(osIndex).toString().toStdString();
                queryDetails.specs = specs;
            }
            queryDetails.address = query.value(addressIndex).toString().toStdString();
            queryDetails.public_key = query.value(keyIndex).toString().toStdString();
            queryDetails.id = id;
            queryDetails.empty = false;
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


// Test checks if the addWorker method in Gateway works properly given correct parameters
TEST_F(AddWorkerTest, AddWorkerTest_AddFirstWorkerSuccess_Test){

    // The first entry's id should be 1
    EXPECT_TRUE(WorkerGateway::add(details) == 1);

    // The add must be successful
    EXPECT_TRUE(wasWorkerAddSuccessful(details, 1));
}

// Test to see if the auto increment feature works as expected.
// Adds the workers from the AddWorkerTest fixture
TEST_F(AddWorkerTest, AddWorkerTest_AddSecondWorkerSuccess_Test){

    // Add the worker from the first test. Since it's the first worker, its id should be 1.
    EXPECT_TRUE(WorkerGateway::add(details) == 1);
    EXPECT_TRUE(wasWorkerAddSuccessful(details, 1));

    // Initialize a new worker
    worker_details seconddetails{};
    seconddetails.public_key = "sadfjsaljdf";
    seconddetails.specs = details.specs;
    seconddetails.address = "1.2.3.4";
    seconddetails.name = "Ubuntu";
    seconddetails.id = 2;
    seconddetails.empty = false;

    EXPECT_TRUE(WorkerGateway::add(seconddetails) == 2);
    EXPECT_TRUE(wasWorkerAddSuccessful(seconddetails, 2));
}

/**
 * Fixture class that deletes the workers table on setup and restores it on teardown.
 */
class NoWorkersTableTest : public ::testing::Test{
protected:
    void SetUp() override {
        // Deletes the workers table
        QSqlQuery query("DROP TABLE workers", IGateway::AcquireDatabase());
        query.exec();

        // Setup the varaibles needed
        details.public_key = "34nrhk3hkr";
        Specs specs{};
        specs.osIdentifier = "Ubuntu 18.04.62";
        specs.ram = 16384;
        specs.cores = 4;
        details.specs = specs;
        details.address = "0.0.0.0";
        details.name = "CentOS";
        id = 1;
        details.empty = false;
    }

    void TearDown() override {
        auto db = IGateway::AcquireDatabase();
        QSqlQuery query("CREATE TABLE IF NOT EXISTS `balancedbanana`.`workers`\n"
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
                        "DEFAULT CHARACTER SET = utf8", db);
        query.exec();
    }

    worker_details details;
    uint64_t id{};
};

// Test to see if an exception is thrown when a worker is being added, but no workers' table exists.
TEST_F(NoWorkersTableTest, NoWorkersTableTest_AddWorker_Test){
    EXPECT_THROW(WorkerGateway::add(details), std::logic_error);
}

// Test to see if an exception is thrown when a worker is being removed, but no workers' table exists.
TEST_F(NoWorkersTableTest, NoWorkersTableTest_RemoveWorker_Test){
    EXPECT_THROW(WorkerGateway::remove(id), std::logic_error);
}

// Test to see if an exception is thrown when the worker getter is called, but no workers' table exists.
TEST_F(NoWorkersTableTest, NoWorkersTableTest_GetWorker_Test){
    EXPECT_THROW(WorkerGateway::getWorker(id), std::logic_error);
}

// Test to see if an exception is thrown when the workers getter is called, but no workers' table exists.
TEST_F(NoWorkersTableTest, NoWorkersTableTest_GetWorkers_Test){
    EXPECT_THROW(WorkerGateway::getWorkers(), std::logic_error);
}

/**
 * Check if the remove query on id was successful
 * @param id The id of the removed record.
 * @return  true if remove was successful, otherwise false.
 */
bool wasWorkerRemoveSuccessful(uint64_t id){
    auto db = IGateway::AcquireDatabase();
    QSqlQuery query("SELECT * FROM workers WHERE id = ?", db);
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return !query.next();
    } else {
        qDebug() << "wasWorkerRemoveSuccessful error: " << query.lastError();
        return false;
    }
}

/**
 * Fixture class that resets the auto increment on teardown.
 */
class RemoveWorkerTest : public ::testing::Test {
protected:
    void TearDown() override{
        resetWorkerTable();
    }
};

// Test to see if a worker is removed after successfully.
TEST_F(RemoveWorkerTest, RemoveWorkerTest_SuccessfulRemove_Test){
    // Add a worker
    worker_details details{};
    details.public_key = "34nrhk3hkr";
    Specs specs{};
    specs.osIdentifier = "Kubuntu 18.0123";
    specs.ram = 16384;
    specs.cores = 6;
    details.specs = specs;
    details.address = "0.0.0.0";
    details.name = "CentOS";
    details.id = 1;
    details.empty = false;
    // Since this is the first worker, this has to be true.
    EXPECT_TRUE(WorkerGateway::add(details) == 1);
    EXPECT_TRUE(wasWorkerAddSuccessful(details, 1));

    // This must return true.
    EXPECT_TRUE(WorkerGateway::remove(1));
    EXPECT_TRUE(wasWorkerRemoveSuccessful(1));
}

// Test to see if the remove method fails when it's called with an invalid id.
TEST_F(RemoveWorkerTest, RemoveWorkerTest_FailureRemove_Test){
    EXPECT_FALSE(WorkerGateway::remove(1));
}


/**
 * Fixture class that initializes a sample worker on setUp and resets the table on teardown.
 */
class GetWorkerTest : public ::testing::Test{
protected:
    void SetUp() override {
        details.public_key = "34nrhk3hkr";
        Specs specs{};
        specs.osIdentifier = "UNIX";
        specs.ram = 16384;
        specs.cores = 4;
        details.specs = specs;
        details.address = "0.0.0.0";
        details.name = "CentOS";
        details.id = 1;
        details.empty = false;
    }

    void TearDown() override {
        resetWorkerTable();
    }

    worker_details details;
};

// Test to see if the first worker can be retrieved correctly.
TEST_F(GetWorkerTest, GetWorkerTest_SuccessfulGet_Test){
    // Add the worker. Its id should be 1, since it's the first worker to be added.
    EXPECT_EQ(WorkerGateway::add(details), details.id);

    // Get the worker and compare it to the added worker. They should be equal.
    worker_details expected_details = WorkerGateway::getWorker(details.id);
    EXPECT_TRUE(details == expected_details);
}

// Test to see if the getter method returns an empty worker_details when its called with an invalid id
TEST_F(GetWorkerTest, GetWorkerTest_NonExistentWorker_Test){
    worker_details empty_details{};
    EXPECT_TRUE(WorkerGateway::getWorker(1) == empty_details);
}

/**
 * Fixture class that initializes three workers on setup and resets the table on teardown.
 */
class GetWorkersTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up the first worker
        first.public_key = "34nrhk3hkr";
        Specs firstSpecs{};
        firstSpecs.osIdentifier = "some os";
        firstSpecs.ram = 16384;
        firstSpecs.cores = 4;
        first.specs = firstSpecs;
        first.address = "0.0.0.0";
        first.name = "CentOS";
        first.id = 1;
        first.empty = false;

        // Set up the second worker
        second.public_key = "fsd8iasdf8sadf";
        second.specs = firstSpecs;
        second.specs->ram = 17385;
        second.address = "1.1.1.1";
        second.name = "Ubuntu";
        second.id = 2;
        second.empty = false;

        // Set up the third worker
        third.public_key = "asdfascascsd";
        third.specs = firstSpecs;
        third.specs->cores = 10;
        third.address = "2.2.2.2";
        third.name = "Windows";
        third.id = 3;
        third.empty = false;
    }

    void TearDown() override {
        resetWorkerTable();
    }

    worker_details first;
    worker_details second;
    worker_details third;
};

// Test to see if getWorkers retrieves a vector of previously added workers from the database
TEST_F(GetWorkersTest, GetWorkersTest_SuccessfulGet_Test){
    // Add the workers. Their ids should match the order of their addition.
    EXPECT_EQ(WorkerGateway::add(first), first.id);
    EXPECT_EQ(WorkerGateway::add(second), second.id);
    EXPECT_EQ(WorkerGateway::add(third), third.id);

    std::vector<worker_details> expectedVector;
    expectedVector.push_back(first);
    expectedVector.push_back(second);
    expectedVector.push_back(third);

    std::vector<worker_details> actualVector = WorkerGateway::getWorkers();
    EXPECT_TRUE(Utilities::areDetailVectorsEqual(expectedVector, actualVector));
}

// Test to see if the getter method returns an empty vector if the workers table is empty
TEST_F(GetWorkersTest, GetWorkersTest_NonExistentWorkers_Test){
    EXPECT_TRUE(WorkerGateway::getWorkers().empty());
}

class GetWorkerByNameTest : public ::testing::Test {
protected:
    void SetUp() override {
        worker.public_key = "34nrhk3hkr";
        Specs specs{};
        specs.osIdentifier = "10240";
        specs.ram = 16384;
        specs.cores = 4;
        worker.specs = specs;
        worker.address = "0.0.0.0";
        worker.name = "CentOS";
        worker.id = 1;
        worker.empty = false;
    }

    void TearDown() override {
        resetWorkerTable();
    }

    worker_details worker;
};

TEST_F(GetWorkerByNameTest, GetWorkerByNameTest_NoWorkersTable_Test){
    auto db = IGateway::AcquireDatabase();
    QSqlQuery query("DROP TABLE workers", db);
    query.exec();
    EXPECT_THROW(WorkerGateway::getWorkerByName(worker.name), std::logic_error);
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

TEST_F(GetWorkerByNameTest, GetWorkerByNameTest_WorkerFound_Test){
    EXPECT_EQ(WorkerGateway::add(worker), worker.id);
    EXPECT_TRUE(wasWorkerAddSuccessful(worker, worker.id));
    EXPECT_TRUE(WorkerGateway::getWorkerByName(worker.name) == worker);
}

TEST_F(GetWorkerByNameTest, GetWorkerByNameTest_GetWorkerByNameTest_WorkerNotFound_Test_TestFound_Test){
    EXPECT_EQ(WorkerGateway::getWorkerByName(worker.name).id, 0);
}

class UpdateWorkerTest : public ::testing::Test {
protected:
    void SetUp() override {
        worker.public_key = "34nrhk3hkr";
        Specs specs{};
        specs.osIdentifier = "10240";
        specs.ram = 16384;
        specs.cores = 4;
        worker.specs = specs;
        worker.address = "0.0.0.0";
        worker.name = "CentOS";
        worker.id = 1;
        worker.empty = false;
    }

    void TearDown() override {
        resetWorkerTable();
    }

    worker_details worker;
};

TEST_F(UpdateWorkerTest, UpdateWorkerTest_NoWorkersTable_Test){
    auto db = IGateway::AcquireDatabase();
    QSqlQuery query("DROP TABLE workers", db);
    query.exec();
    EXPECT_THROW(WorkerGateway::updateWorker(worker), std::logic_error);
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

TEST_F(UpdateWorkerTest, UpdateWorkerTest_InvalidId_Test){
    worker.id = 0;
    EXPECT_THROW(WorkerGateway::updateWorker(worker), std::invalid_argument);
}

TEST_F(UpdateWorkerTest, UpdateWorkerTest_NoWorker_Test){
    EXPECT_THROW(WorkerGateway::updateWorker(worker), std::runtime_error);
}

TEST_F(UpdateWorkerTest, UpdateWorkerTest_Success_Test){
    EXPECT_EQ(WorkerGateway::add(worker), worker.id);
    EXPECT_TRUE(wasWorkerAddSuccessful(worker, worker.id));

    worker_details new_worker = worker;
    new_worker.name = "Windows 10";
    WorkerGateway::updateWorker(new_worker);
    worker_details actualWorker = WorkerGateway::getWorker(worker.id);
    EXPECT_TRUE(actualWorker == new_worker);
}

std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

// Generate an unicode string of length 'len' whose characters are in range [start, end]
std::wstring generateRandomUnicodeString(size_t len, size_t start, size_t end)
{
    wchar_t* ustr = new wchar_t[len+1];      // +1 for '\0'
    size_t intervalLength = end - start + 1; // +1 for inclusive range

    srand(time(NULL));
    for (auto i = 0; i < len; i++) {
        ustr[i] = (rand() % intervalLength) + start;
    }
    ustr[len] = L'\0';
    return std::wstring(ustr);
}


TEST(WorkerEncodingTest, WorkerEncodingTest_U8Test_Test){
    /*
    worker_details worker;
    worker.public_key = "safdsadf";
    std::string name = u8"GROẞBUCHSTABEN";
    worker.name = name;
    worker.empty = false;
    worker.id = 1;
    EXPECT_EQ(WorkerGateway::add(worker), worker.id);
    worker_details actualWorker = WorkerGateway::getWorker(worker.id);
    EXPECT_EQ(WorkerGateway::getWorker(worker.id).name, worker.name);
    qDebug() << QString::fromStdString(actualWorker.name) << QString::fromStdString(worker.name);

     */
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<uint32_t>::max());
    std::vector<uint32_t> name(4);
    for (auto &&i : name) {
        i = dis(gen);
    }
    std::string sname((char*)name.data(), name.size() * sizeof(uint32_t));

    std::wstring output = generateRandomUnicodeString(5, 0x0400, 0x04FF);
    auto db = IGateway::AcquireDatabase();
    QSqlQuery query("INSERT INTO workers (name, public_key) VALUES (?,?)", db);
    query.addBindValue(QString::fromStdWString(output));
    query.addBindValue("something");
    query.exec();
    query.prepare("SELECT name FROM workers WHERE id = 1");
    if (query.exec() && query.next()){
        EXPECT_EQ(query.value(0).toString().toStdWString(), output);
    }

    resetWorkerTable();
}

