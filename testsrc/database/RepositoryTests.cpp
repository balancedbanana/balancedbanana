#include <gtest/gtest.h>
#include <database/Repository.h>

using namespace balancedbanana::database;

class RepositoryTest : public testing::Test {
protected:
    Repository *repo;

    RepositoryTest() : repo(nullptr) {
    }

    void SetUp() override {
        repo = new Repository("localhost", "balancedbanana", "balancedbanana", "qwer1234", 3306, std::chrono::seconds(1));
    }

    void TearDown() override {
        delete repo;
    }

};
