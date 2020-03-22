#include <gtest/gtest.h>
#include <database/Utilities.h>
#include <QSqlQuery>

#include "DatabaseTest.h"

using namespace balancedbanana::database;

class RepositoryTest : public DatabaseTest {
protected:

    void SetUp() override {
        DatabaseTest::SetUp();
        ClearDatabase();
    }

};

TEST_F(RepositoryTest, AddUser_GetUser) {
    EXPECT_EQ(repo->GetUser(0), nullptr);
    EXPECT_NO_THROW(repo->AddUser(1, "name", "email", "publickey"));
    auto ptr = repo->GetUser(1);
    EXPECT_NE(ptr, nullptr);
    ptr = nullptr;
    repo->WriteBack();
    repo->ClearCache();
    ptr = repo->GetUser(1);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->name(), "name");
    EXPECT_EQ(ptr->email(), "email");
    EXPECT_EQ(ptr->pubkey(), "publickey");
}

TEST_F(RepositoryTest, AddJob_GetJob) {
    EXPECT_EQ(repo->GetJob(0), nullptr);
    repo->AddUser(1, "name", "email2", "publickey");
    std::shared_ptr<Job> job = nullptr;
    JobConfig config;
    config.set_current_working_dir("working_dir");
    config.set_blocking_mode(true);
    config.set_min_ram(123456);
    QDateTime time;
    EXPECT_NO_THROW(job = repo->AddJob(1, config, time, "command"));
    EXPECT_NE(job, nullptr);
    uint64_t id = job->getId();
    auto ptr = repo->GetJob(id);
    EXPECT_NE(ptr, nullptr);
    ptr = nullptr;
    repo->WriteBack();
    repo->ClearCache();
    ptr = repo->GetJob(id);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->getUser()->id(), 1);
    EXPECT_EQ(ptr->getConfig()->current_working_dir(), "working_dir");
    EXPECT_EQ(ptr->getConfig()->blocking_mode(), true);
    EXPECT_EQ(ptr->getConfig()->min_ram(), 123456);
    EXPECT_EQ(ptr->getConfig()->max_cpu_count(), std::nullopt);
    EXPECT_EQ(ptr->getScheduled_at(), time);
    EXPECT_EQ(ptr->getCommand(), "command");
}

TEST_F(RepositoryTest, AddWorker_GetWorker) {
    EXPECT_EQ(repo->GetWorker(0), nullptr);
    Specs specs;
    specs.ram = 1234;
    specs.cores = 42;
    specs.osIdentifier = "testOs";
    std::shared_ptr<Worker> worker = nullptr;
    EXPECT_NO_THROW(worker = repo->AddWorker("name", "publickey", specs, "address"));
    EXPECT_NE(worker, nullptr);
    uint64_t id = worker->getId();
    auto ptr = repo->GetWorker(id);
    EXPECT_NE(ptr, nullptr);
    ptr = nullptr;
    repo->FlushCache();
    repo->ClearCache();
    ptr = repo->GetWorker(id);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->name(), "name");
    EXPECT_EQ(ptr->pubkey(), "publickey");
    EXPECT_TRUE(ptr->getSpec());
    EXPECT_EQ(ptr->getSpec()->ram, specs.ram);
    EXPECT_EQ(ptr->getSpec()->cores, specs.cores);
    EXPECT_EQ(ptr->getSpec()->osIdentifier, specs.osIdentifier);
    EXPECT_EQ(ptr->getAddress(), "address");
}