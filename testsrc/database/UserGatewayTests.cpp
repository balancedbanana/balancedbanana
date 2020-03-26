#include <gtest/gtest.h>
#include <database/UserGateway.h>
#include <database/user_details.h>
#include <database/Repository.h>
#include <database/Utilities.h>

#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>

#include "DatabaseTest.h"

using namespace balancedbanana::database;

using UserGatewayTest = DatabaseTest;

/**
 * Deletes the all records in the users table for the id.
 */
void resetUserTable(const std::shared_ptr<QSqlDatabase>& db){
    QSqlQuery query("DELETE FROM users", *db);
    query.exec();
}

/**
 * Fixture class that initializes a sample user's details.
 */
class AddUserTest : public UserGatewayTest {
protected:
    user_details details;

    void SetUp() override {
        UserGatewayTest::SetUp();
        details.id = 1;
        details.public_key = "asdjfascd3";
        details.email = "balanced@banana.kit.edu";
        details.name = "Rakan";
        details.empty = false;
    }

    void TearDown() override {
        resetUserTable(db);
    }
};

/**
 * Checks if the addUser query was successful.
 * @param details The record details that were added with the query.
 * @param id The id of the added record.
 * @return true if the addUser was successful, otherwise false.
 */
bool wasUserAddSuccessful(const user_details& details, uint64_t id, const std::shared_ptr<QSqlDatabase>& db){
    QSqlQuery query("SELECT * FROM users WHERE id = ?", *db);
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        if (query.next()){
            int nameIndex = query.record().indexOf("name");
            int emailIndex = query.record().indexOf("email");
            int keyIndex = query.record().indexOf("public_key");
            user_details queryDetails{};
            queryDetails.name = query.value(nameIndex).toString().toStdString();
            queryDetails.email = query.value(emailIndex).toString().toStdString();
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


// Test checks if the addUser method works properly given correct parameters
TEST_F(AddUserTest, AddUserTest_AddFirstUserSuccess_Test){

    // The first entry's id should be 1
    EXPECT_NO_THROW(userGateway->addUser(details));

    // The add must be successful
    EXPECT_TRUE(wasUserAddSuccessful(details, 1, db));
}

// Test to see if the auto increment feature works as expected.
TEST_F(AddUserTest, AddUserTest_AddSecondUserSucess_Test){

    // Add the user from the first test. Since it's the first user, its id should be 1.
    EXPECT_NO_THROW(userGateway->addUser(details));
    EXPECT_TRUE(wasUserAddSuccessful(details, 1, db));

    // Initialize a new user
    user_details seconddetails{};
    seconddetails.public_key = "sadfjsaljdf";
    seconddetails.email = "example@email.com";
    seconddetails.name = "John";
    seconddetails.empty = false;
    seconddetails.id = 2;

    EXPECT_NO_THROW(userGateway->addUser(seconddetails));
    EXPECT_TRUE(wasUserAddSuccessful(seconddetails, 2, db));
}


/**
 * Fixture class that deletes the users table on setup and restores it on teardown.
 */
class NoUsersTableTest : public UserGatewayTest {
protected:
    user_details details;

    void SetUp() override {
        UserGatewayTest::SetUp();
        // Deletes the users table
        QSqlQuery query("DROP TABLE users", *db);
        query.exec();

        // Setup the varaibles needed
        details.public_key = "34nrhk3hkr";
        details.email = "someemail@google.com";
        details.name = "Max";
        details.id = 1;
        details.empty = false;
    }

    void TearDown() override {
        QSqlQuery query("CREATE TABLE `users` (\n"
                        "  `name` varchar(45) NOT NULL,\n"
                        "  `email` varchar(255) NOT NULL,\n"
                        "  `public_key` longtext NOT NULL,\n"
                        "  `id` bigint(10) unsigned NOT NULL,\n"
                        "  PRIMARY KEY (`id`),\n"
                        "  UNIQUE KEY `id_UNIQUE` (`id`)\n"
                        ") ENGINE=InnoDB DEFAULT CHARSET=utf8", *db);
        query.exec();
    }
};

// Test to see if an exception is thrown when a user is being added, but no users' table exists.
TEST_F(NoUsersTableTest, NoUsersTableTest_AddUser_Test){
    EXPECT_THROW(userGateway->addUser(details), std::logic_error);
}

// Test to see if an exception is thrown when a user is being removed, but no users' table exists.
TEST_F(NoUsersTableTest, NoUsersTableTest_RemoveUser_Test){
    EXPECT_THROW(userGateway->removeUser(details.id), std::logic_error);
}

// Test to see if an exception is thrown when the user getter is called, but no users' table exists.
TEST_F(NoUsersTableTest, NoUsersTableTest_GetUser_Test){
    EXPECT_THROW(userGateway->getUser(details.id), std::logic_error);
}

// Test to see if an exception is thrown when the users getter is called, but no users' table exists.
TEST_F(NoUsersTableTest, NoUsersTableTest_GetUsers_Test){
    EXPECT_THROW(userGateway->getUsers(), std::logic_error);
}

/**
 * Check if the remove query on id was successful
 * @param id The id of the removed record.
 * @return  true if remove was successful, otherwise false.
 */
bool wasUserRemoveSuccessful(uint64_t id, const std::shared_ptr<QSqlDatabase>& db){
    QSqlQuery query("SELECT * FROM users WHERE id = ?", *db);
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return !query.next();
    } else {
        qDebug() << "wasUserRemoveSuccessful error: " << query.lastError();
        return false;
    }
}

/**
 * Fixture class that resets the auto increment on teardown.
 */
class RemoveUserTest : public UserGatewayTest {
protected:
    void TearDown() override{
        resetUserTable(db);
    }
};

// Test to see if a user is removed after successfully.
TEST_F(RemoveUserTest, RemoveUserTest_SuccessfulRemove_Test){
    // Add a user
    user_details details{};
    details.public_key = "34nrhk3hkr";
    details.email = "email@email.com";
    details.name = "CentOS";
    details.empty = false;
    details.id = 1;
    // Since this is the first user, this has to be true.
    EXPECT_NO_THROW(userGateway->addUser(details));
    EXPECT_TRUE(wasUserAddSuccessful(details, 1, db));

    // This must return true.
    EXPECT_NO_THROW(userGateway->removeUser(1));
    EXPECT_TRUE(wasUserRemoveSuccessful(1, db));
}

// Test to see if the remove method fails when it's called with an invalid id.
TEST_F(RemoveUserTest, RemoveUserTest_FailureRemove_Test){
    EXPECT_THROW(userGateway->removeUser(1), std::runtime_error);
}

/**
 * Fixture class that initializes a sample user on setUp and resets the table on teardown.
 */
class GetUserTest : public UserGatewayTest{
protected:
    void SetUp() override {
        UserGatewayTest::SetUp();
        details.public_key = "34nrhk3hkr";
        details.email = "email@email.com";
        details.name = "CentOS";
        details.id = 1;
        details.empty = false;
    }

    void TearDown() override {
        resetUserTable(db);
    }

    user_details details;
};

// Test to see if the first user can be retrieved correctly.
TEST_F(GetUserTest, GetUserTest_SuccessfulGet_Test){
    // Add the user. Its id should be 1, since it's the first to be added.
    EXPECT_NO_THROW(userGateway->addUser(details));

    // Get the user and compare it to the added user. They should be equal.
    user_details expected_details = userGateway->getUser(details.id);
    EXPECT_TRUE(details == expected_details);
}

// Test to see if the getter method returns an empty user_details when its called with an invalid id
TEST_F(GetUserTest, GetUserTest_NonExistentUser_Test){
    user_details empty_details{};
    EXPECT_TRUE(userGateway->getUser(1) == empty_details);
}

/**
 * Fixture class that initializes three users on setup and resets the table on teardown.
 */
class GetUsersTest : public UserGatewayTest {
protected:
    void SetUp() override {
        UserGatewayTest::SetUp();
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

        // Set up the third user
        third.public_key = "asdfascascsd";
        third.email = "anotheremail@kit.edu";
        third.name = "Windows";
        third.id = 3;
        third.empty = false;
    }

    void TearDown() override {
        resetUserTable(db);
    }

    user_details first;
    user_details second;
    user_details third;
};

// Test to see if getUsers retrieves a vector of previously added users from the database
TEST_F(GetUsersTest, GetUsersTest_SuccessfulGet_Test){
    // Add the users. Their ids should match the order of their addition.
    EXPECT_NO_THROW(userGateway->addUser(first));
    EXPECT_NO_THROW(userGateway->addUser(second));
    EXPECT_NO_THROW(userGateway->addUser(third));

    std::vector<user_details> expectedVector;
    expectedVector.push_back(first);
    expectedVector.push_back(second);
    expectedVector.push_back(third);

    std::vector<user_details> actualVector = userGateway->getUsers();
    EXPECT_TRUE(Utilities::areDetailVectorsEqual(expectedVector, actualVector));
}

// Test to see if the getter method returns an empty vector if the users table is empty
TEST_F(GetUsersTest, GetUsersTest_NonExistentUsers_Test){
    EXPECT_TRUE(userGateway->getUsers().empty());
}

class GetUserByNameTest : public UserGatewayTest {
protected:
    void SetUp() override {
        UserGatewayTest::SetUp();
        // Set up the first user
        user.public_key = "34nrhk3hkr";
        user.email = "someemail@kit.edu";
        user.name = "Rakan";
        user.id = 1;
        user.empty = false;
    }

    void TearDown() override {
        resetUserTable(db);
    }

    user_details user;
};

TEST_F(GetUserByNameTest, GetUserByNameTest_NoUsersTable_Test){
    QSqlQuery query("DROP TABLE users", *db);
    query.exec();
    EXPECT_THROW(userGateway->getUserByName(user.name), std::logic_error);
    query.prepare("CREATE TABLE `users` (\n"
                    "  `name` varchar(45) NOT NULL,\n"
                    "  `email` varchar(255) NOT NULL,\n"
                    "  `public_key` longtext NOT NULL,\n"
                    "  `id` bigint(10) unsigned NOT NULL,\n"
                    "  PRIMARY KEY (`id`),\n"
                    "  UNIQUE KEY `id_UNIQUE` (`id`)\n"
                    ") ENGINE=InnoDB DEFAULT CHARSET=utf8");
    query.exec();
}

TEST_F(GetUserByNameTest, GetUserByNameTest_UserFound_Test){
    EXPECT_NO_THROW(userGateway->addUser(user));
    EXPECT_TRUE(wasUserAddSuccessful(user, user.id, db));
    EXPECT_TRUE(userGateway->getUserByName(user.name) == user);
}

TEST_F(GetUserByNameTest, GetUserByNameTest_UserNotFound_Test){
    EXPECT_EQ(userGateway->getUserByName(user.name).id, 0);
}

class UpdateUserTest : public UserGatewayTest {
protected:
    void SetUp() override {
        UserGatewayTest::SetUp();
        user.public_key = "34nrhk3hkr";
        user.email = "someemail@kit.edu";
        user.name = "Rakan";
        user.id = 1;
        user.empty = false;
    }

    void TearDown() override {
        resetUserTable(db);
    }

    user_details user;
};

TEST_F(UpdateUserTest, UpdateUserTest_NoUsersTable_Test){
    QSqlQuery query("DROP TABLE users", *db);
    query.exec();
    EXPECT_THROW(userGateway->updateUser(user), std::logic_error);
    query.prepare("CREATE TABLE `users` (\n"
                  "  `name` varchar(45) NOT NULL,\n"
                  "  `email` varchar(255) NOT NULL,\n"
                  "  `public_key` longtext NOT NULL,\n"
                  "  `id` bigint(10) unsigned NOT NULL,\n"
                  "  PRIMARY KEY (`id`),\n"
                  "  UNIQUE KEY `id_UNIQUE` (`id`)\n"
                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8");
    query.exec();
}

TEST_F(UpdateUserTest, UpdateUserTest_InvalidId_Test){
    user.id = 0;
    EXPECT_THROW(userGateway->updateUser(user), std::invalid_argument);
}

TEST_F(UpdateUserTest, UpdateUserTest_NoUser_Test){
    EXPECT_THROW(userGateway->updateUser(user), std::runtime_error);
}

TEST_F(UpdateUserTest, UpdateUserTest_Success_Test){
    EXPECT_NO_THROW(userGateway->addUser(user));
    EXPECT_TRUE(wasUserAddSuccessful(user, user.id, db));
    // Add new details
    user_details new_user;
    new_user.public_key = "asdjcdasdlkcsadl";
    new_user.empty = false;
    new_user.email = "someemail@kit.edu";
    new_user.name = "John";
    new_user.id = 1;
    userGateway->updateUser(new_user);
    user_details actualUser = userGateway->getUser(new_user.id);
    EXPECT_TRUE(actualUser == new_user);
}


