//
// Created by Rakan Al Masri on 30.01.20.
//

#include <gtest/gtest.h>
#include <database/Gateway.h>
#include <database/worker_details.h>

using namespace balancedbanana::database;

Gateway* gateway;

/**
 * Test environment that has a global SetUp and TearDown methods for all test suites/tests.
 *
 * Sets up the global Gateway variable to access the methods.
 *
 */
class GatewayEnvironment : public ::testing::Environment{
public:
    void SetUp() override {
        gateway = new Gateway();
    }

    void TearDown() override {
        delete gateway;
        gateway = nullptr;
    }
};

GatewayEnvironment gateway_env;

/**
 * Test to see if the connection to the DB was correctly established.
 */
TEST(ConnectionTest, ConnectionTest_CheckDBConnection_Test){}

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
    ASSERT_TRUE((*gateway).addWorker(details.public_key, details.specs.space, details.specs.ram, details.specs.cores,
            details.address, details.name) == 1);
}

int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new GatewayEnvironment);
    return RUN_ALL_TESTS();
}