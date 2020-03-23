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

TEST_F(RepositoryTest, GetRepository) {
    EXPECT_EQ(Repository::GetRepository("balancedbanana", "localhost", "balancedbanana", "balancedbanana", "qwer1234", 3306), repo);
    EXPECT_EQ(Repository::GetRepository("balancedbanana"), repo);
    EXPECT_THROW(Repository::GetRepository("invalid database name"), std::runtime_error);
}

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

TEST_F(RepositoryTest, AlterUser) {
    std::shared_ptr<User> user = nullptr;
    EXPECT_NO_THROW(user = repo->AddUser(1, "name", "email", "public key"));
    EXPECT_NE(user, nullptr);
    user->setEmail("new email");
    repo->WriteBack();
    repo->ClearCache();
    user = nullptr;
    EXPECT_NO_THROW(user = repo->GetUser(1));
    EXPECT_NE(user, nullptr);
    EXPECT_EQ(user->email(), "new email");
    repo->ClearCache();

}

TEST_F(RepositoryTest, FindUser) {
    EXPECT_NO_THROW(repo->AddUser(1, "user 1", "email 1", "public key 1"));
    EXPECT_NO_THROW(repo->AddUser(2, "user 2", "email 2", "public key 2"));
    EXPECT_NO_THROW(repo->AddUser(3, "user 3", "email 3", "public key 3"));
    EXPECT_NO_THROW(repo->AddUser(4, "user 4", "email 4", "public key 4"));
    EXPECT_NO_THROW(repo->AddUser(5, "user 5", "email 5", "public key 5"));
    EXPECT_NO_THROW(repo->AddUser(6, "user 6", "email 6", "public key 6"));
    EXPECT_EQ(repo->FindUser("user 3")->id(), 3);
    EXPECT_EQ(repo->FindUser("user 5")->id(), 5);
    repo->WriteBack();
    repo->ClearCache();
    EXPECT_EQ(repo->FindUser("user 4")->id(), 4);
    EXPECT_EQ(repo->FindUser("invalid user"), nullptr);
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

TEST_F(RepositoryTest, AlterJob_ChangeStatus) {
    auto user = repo->AddUser(1, "name", "email", "public key");
    std::shared_ptr<Job> job = nullptr;
    QDateTime time = QDateTime::currentDateTime();
    JobConfig config;
    EXPECT_NO_THROW(job = repo->AddJob(1, config, time, "command"));
    EXPECT_NE(job, nullptr);
    uint64_t id = job->getId();
    job->setStatus(canceled);
    QDateTime finishTime = QDateTime::currentDateTime();
    job->setFinished_at(finishTime);
    repo->WriteBack();
    repo->ClearCache();
    auto newJob = job;
    EXPECT_NO_THROW(job = repo->GetJob(id));
    EXPECT_NE(newJob, nullptr);
    EXPECT_EQ(newJob->getStatus(), canceled);
    EXPECT_EQ(newJob->getFinished_at(), finishTime);
    EXPECT_NE(job, newJob);
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

TEST_F(RepositoryTest, TimedWriteBack) {
    auto user = repo->AddUser(1, "name", "my super unique email", "public key");
    std::this_thread::sleep_for(std::chrono::seconds(3));
    repo->ClearCache();
    user = nullptr;
    EXPECT_NO_THROW(user = repo->GetUser(1));
    EXPECT_NE(user, nullptr);
    EXPECT_EQ(user->name(), "name");
    std::cout << user->email() << std::endl;
}

TEST_F(RepositoryTest, FlushCache) {
    auto user = repo->AddUser(1, "name", "email", "public key");
    user->setEmail("new email");
    repo->FlushCache();
    auto newUser = repo->GetUser(1);
    EXPECT_EQ(user, newUser);
    auto usrptr = (uint64_t) user.get();
    user = nullptr;
    newUser = nullptr;
    repo->FlushCache();
    user = repo->GetUser(1);
    EXPECT_NE((uint64_t) user.get(), usrptr);
}