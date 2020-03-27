#pragma once

#include <gtest/gtest.h>
#include <database/Repository.h>

using namespace balancedbanana::database;



class DatabaseTest : public testing::Test {
protected:
    std::shared_ptr<Repository> repo;
    std::string dbName;
    std::shared_ptr<JobGateway> jobGateway;
    std::shared_ptr<UserGateway> userGateway;
    std::shared_ptr<WorkerGateway> workerGateway;

    void ClearDatabase() {
        auto db = QSqlDatabase::database(QString::fromStdString(dbName));

        QSqlQuery("DELETE FROM jobs", db).exec();
        QSqlQuery("DELETE FROM workers", db).exec();
        QSqlQuery("DELETE FROM users", db).exec();
        QSqlQuery("DELETE FROM job_results", db).exec();
        QSqlQuery("DELETE FROM allocated_resources", db).exec();
        QSqlQuery("ALTER TABLE jobs AUTO_INCREMENT = 1", db).exec();
        QSqlQuery("ALTER TABLE workers AUTO_INCREMENT = 1", db).exec();
        repo->ClearCache();
    }

    void SetUp() override {
        repo = Repository::GetRepository("balancedbanana", "localhost", "balancedbanana", "balancedbanana", "qwer1234", 3306, std::chrono::seconds(1));
        dbName = repo->GetDatabase();
        jobGateway = std::make_shared<JobGateway>(dbName);
        userGateway = std::make_shared<UserGateway>(dbName);
        workerGateway = std::make_shared<WorkerGateway>(dbName);
    }

    void TearDown() override {
        repo->ClearCache();
        ClearDatabase();
    }
};