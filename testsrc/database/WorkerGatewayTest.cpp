#include <gtest/gtest.h>
#include <database/WorkerGateway.h>
#include <database/worker_details.h>
#include <database/Repository.h>
#include <QSqlQuery>

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
        Repository();

    }
};

WorkerGatewayEnvironment gateway_env;

/**
 * Test to see if the connection to the DB was correctly established.
 */
TEST(ConnectionTest, ConnectionTest_CheckkDBConnection_Test){}

/**
 * Creates a fixture class that initializes a sample worker's details.
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

    worker_details details;
};

// Test checks if the addWorker method in Gateway works properly given correct parameters
TEST_F(AddWorkerTest, AddWorkerTest_AddSingleWorkerSuccess_Test){

    // To reset the AUTO_INCREMENT, it has to be removed and then added back
    QSqlQuery query("ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL");
    query.exec();
    query.prepare("DELETE FROM workers");
    query.exec();
    query.prepare("ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT");
    query.exec();

    // The first entry's id should be 1
    ASSERT_TRUE(WorkerGateway::add(details) == 1);
}

int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new WorkerGatewayEnvironment);
    return RUN_ALL_TESTS();
}