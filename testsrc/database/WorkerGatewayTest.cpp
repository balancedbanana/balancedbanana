#include <gtest/gtest.h>
#include <database/WorkerGateway.h>
#include <database/worker_details.h>
#include <database/Repository.h>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>

using namespace balancedbanana::database;


/**
 * Test environment that has a global SetUp and TearDown methods for all test suites/tests.
 *
 * Sets up the global Gateway variable to access the methods.
 *
 */
class WorkerGatewayEnvironment : public ::testing::Environment{
public:
    void SetUp() override {
        Repository("127.0.0.1", "balancedbanana", "root", "banana", 3306);
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
void resetTableAI(){
    QSqlQuery query("ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL");
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
        details.specs.space = 10240;
        details.specs.ram = 16384;
        details.specs.cores = 4;
        details.address = "0.0.0.0";
        details.name = "CentOS";
    }

    void TearDown() override {
        resetTableAI();
    }

    worker_details details;
};

/**
 * Checks if the add query was successful.
 * @param details The record details that were added with the query.
 * @param id The id of the added record.
 * @return true if the add was successful, otherwise false.
 */
bool wasAddSuccessful(const worker_details& details, uint64_t id){
    QSqlQuery query("SELECT * FROM workers WHERE id = ?");
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        if (query.next()){
            int nameIndex = query.record().indexOf("name");
            int ramIndex = query.record().indexOf("ram");
            int coresIndex = query.record().indexOf("cores");
            int spaceIndex = query.record().indexOf("space");
            int addressIndex = query.record().indexOf("address");
            int keyIndex = query.record().indexOf("public_key");
            EXPECT_EQ(query.value(nameIndex).toString().toStdString(), details.name);
            EXPECT_EQ(query.value(ramIndex).toUInt(), details.specs.ram);
            EXPECT_EQ(query.value(coresIndex).toUInt(), details.specs.cores);
            EXPECT_EQ(query.value(spaceIndex).toUInt(), details.specs.space);
            EXPECT_EQ(query.value(addressIndex).toString().toStdString(), details.address);
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


// Test checks if the addWorker method in Gateway works properly given correct parameters
TEST_F(AddWorkerTest, AddWorkerTest_AddFirstWorkerSuccess_Test){

    // The first entry's id should be 1
    ASSERT_TRUE(WorkerGateway::add(details) == 1);

    // The add must be successful
    ASSERT_TRUE(wasAddSuccessful(details, 1));
}

// Test to see if the auto increment feature works as expected.
// Adds the workers from the AddWorkerTest fixture
TEST_F(AddWorkerTest, AddWorkerTest_AddSecondWorkerSucess_Test){

    // Add the worker from the first test. Since it's the first worker, its id should be 1.
    ASSERT_TRUE(WorkerGateway::add(details) == 1);
    ASSERT_TRUE(wasAddSuccessful(details, 1));

    // Initialize a new worker
    worker_details seconddetails{};
    seconddetails.public_key = "sadfjsaljdf";
    seconddetails.specs = details.specs;
    seconddetails.address = "1.2.3.4";
    seconddetails.name = "Ubuntu";

    ASSERT_TRUE(WorkerGateway::add(seconddetails) == 2);
    ASSERT_TRUE(wasAddSuccessful(seconddetails, 2));
}

// Test to see if the addWorker method throws an exception when the key arg is invalid.
TEST_F(AddWorkerTest, AddWorkerTest_InvalidKeyArg_Test){
    worker_details detailscpy = details;
    detailscpy.public_key = "";
    ASSERT_THROW(WorkerGateway::add(detailscpy), std::invalid_argument);
}

// Test to see if the addWorker method throws an exception when the space arg is invalid.
TEST_F(AddWorkerTest, AddWorkerTest_InvalidSpaceArg_Test){
    worker_details detailscpy = details;
    detailscpy.specs.space = 0;
    ASSERT_THROW(WorkerGateway::add(detailscpy), std::invalid_argument);
}

// Test to see if the addWorker method throws an exception when the ram arg is invalid.
TEST_F(AddWorkerTest, AddWorkerTest_InvalidRAMArg_Test){
    worker_details detailscpy = details;
    detailscpy.specs.ram = 0;
    ASSERT_THROW(WorkerGateway::add(detailscpy), std::invalid_argument);
}

// Test to see if the addWorker method throws an exception when the cores arg is invalid.
TEST_F(AddWorkerTest, AddWorkerTest_InvalidCoresArg_Test){
    worker_details detailscpy = details;
    detailscpy.specs.cores = 0;
    ASSERT_THROW(WorkerGateway::add(detailscpy), std::invalid_argument);
}

// Test to see if the addWorker method throws an exception when the address arg is invalid.
TEST_F(AddWorkerTest, AddWorkerTest_InvalidAddressArg_Test){
    worker_details detailscpy = details;
    detailscpy.address = "";
    ASSERT_THROW(WorkerGateway::add(detailscpy), std::invalid_argument);
}

// Test to see if the addWorker method throws an exception when the name arg is invalid.
TEST_F(AddWorkerTest, AddWorkerTest_InvalidNameArg_Test){
    worker_details detailscpy = details;
    detailscpy.name = "";
    ASSERT_THROW(WorkerGateway::add(detailscpy), std::invalid_argument);
}

/**
 * Fixture class that deletes the workers table on setup and restores it on teardown.
 */
class NoTableTest : public ::testing::Test{
protected:
    void SetUp() override {
        // Deletes the workers table
        QSqlQuery query("DROP TABLE workers");
        query.exec();

        // Setup the varaibles needed
        details.public_key = "34nrhk3hkr";
        details.specs.space = 10240;
        details.specs.ram = 16384;
        details.specs.cores = 4;
        details.address = "0.0.0.0";
        details.name = "CentOS";
        id = 1;
    }

    void TearDown() override {
        QSqlQuery query("CREATE TABLE `workers` (`id` bigint(10) unsigned NOT NULL AUTO_INCREMENT, `ram` bigint(10) "
                        "unsigned DEFAULT NULL, `cores` bigint(10) unsigned DEFAULT NULL,`space` bigint(10) unsigned "
                        "DEFAULT NULL, `address` varchar(255) DEFAULT NULL, `public_key` varchar(255) DEFAULT NULL, "
                        "`name` varchar(45) DEFAULT NULL, PRIMARY KEY (`id`), UNIQUE KEY `id_UNIQUE` (`id`), UNIQUE "
                        "KEY `public_key_UNIQUE` (`public_key`), UNIQUE KEY `address_UNIQUE` (`address`) ) "
                        "ENGINE=InnoDB DEFAULT CHARSET=latin1");
        query.exec();
    }

    worker_details details;
    uint64_t id;
};

// Test to see if an exception is thrown when a worker is being added, but no workers' table exists.
TEST_F(NoTableTest, NoTableTest_AddWorker_Test){
    EXPECT_THROW(WorkerGateway::add(details), std::logic_error);
}

// Test to see if an exception is thrown when a worker is being removed, but no workers' table exists.
TEST_F(NoTableTest, NoTableTest_RemoveWorker_Test){
    EXPECT_THROW(WorkerGateway::remove(id), std::logic_error);
}

// Test to see if an exception is thrown when the worker getter is called, but no workers' table exists.
TEST_F(NoTableTest, NoTableTest_GetWorker_Test){
    EXPECT_THROW(WorkerGateway::getWorker(id), std::logic_error);
}

// Test to see if an exception is when the workers getter is called, but no workers' table exists.
TEST_F(NoTableTest, NoTableTest_GetWorkers_Test){
    EXPECT_THROW(WorkerGateway::getWorkers(), std::logic_error);
}

/**
 * Check if the remove query on id was successful
 * @param id The id of the removed record.
 * @return  true if remove was successful, otherwise false.
 */
bool wasRemoveSuccessful(uint64_t id){
    QSqlQuery query("SELECT * FROM workers WHERE id = ?");
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        if (query.next()){
            return false;
        } else {
            return true;
        }
    } else {
        qDebug() << "wasRemoveSuccessful error: " << query.lastError();
        return false;
    }
}

/**
 * Fixture class that resets the auto increment on teardown.
 */
class RemoveWorkerTest : public ::testing::Test {
protected:
    void TearDown() override{
        resetTableAI();
    }
};

// Test to see if a worker is removed after successfully.
TEST_F(RemoveWorkerTest, RemoveWorkerTest_SuccessfulRemove_Test){
    // Add a worker
    worker_details details{};
    details.public_key = "34nrhk3hkr";
    details.specs.space = 10240;
    details.specs.ram = 16384;
    details.specs.cores = 4;
    details.address = "0.0.0.0";
    details.name = "CentOS";
    // Since this is the first worker, this has to be true.
    ASSERT_TRUE(WorkerGateway::add(details) == 1);
    ASSERT_TRUE(wasAddSuccessful(details, 1));

    // This must return true.
    ASSERT_TRUE(WorkerGateway::remove(1));
    ASSERT_TRUE(wasRemoveSuccessful(1));
}

// Test to see if the remove method fails when it's called with an invalid id.
TEST_F(RemoveWorkerTest, RemoveWorkerTest_FailureRemove_Test){
    ASSERT_FALSE(WorkerGateway::remove(1));
}

