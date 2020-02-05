#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <configfiles/Priority.h>

using namespace balancedbanana::configfiles;

TEST(PriorityTests, PrioritiesToStrings) {
    EXPECT_EQ("low", to_string(Priority::low));
    EXPECT_EQ("normal", to_string(Priority::normal));
    EXPECT_EQ("high", to_string(Priority::high));
    EXPECT_EQ("emergency", to_string(Priority::emergency));
}

TEST(PriorityTests, NamesToPriorities) {
    bool success = false;
    EXPECT_EQ(Priority::low, stopriority("low", success));
    EXPECT_EQ(true, success);
    success = false;
    EXPECT_EQ(Priority::normal, stopriority("normal", success));
    EXPECT_EQ(true, success);
    success = false;
    EXPECT_EQ(Priority::high, stopriority("high", success));
    EXPECT_EQ(true, success);
    success = false;
    EXPECT_EQ(Priority::emergency, stopriority("emergency", success));
    EXPECT_EQ(true, success);
}

TEST(PriorityTests, NumbersToPriorities) {
    bool success = false;
    EXPECT_EQ(Priority::low, stopriority("1", success));
    EXPECT_EQ(true, success);
    success = false;
    EXPECT_EQ(Priority::normal, stopriority("2", success));
    EXPECT_EQ(true, success);
    success = false;
    EXPECT_EQ(Priority::high, stopriority("3", success));
    EXPECT_EQ(true, success);
    success = false;
    EXPECT_EQ(Priority::emergency, stopriority("4", success));
    EXPECT_EQ(true, success);
}

TEST(PriorityTests, InvalidNameToPriority) {
    bool success = true;
    stopriority("invalid", success);
    EXPECT_EQ(false, success);
}

TEST(PriorityTests, InvalidNumberToPriority) {
    bool success = true;
    stopriority("0", success);
    EXPECT_EQ(false, success);
}

TEST(PriorityTests, InvalidPriorityToString) {
    EXPECT_THROW(to_string((Priority) 5), std::invalid_argument);
}

TEST(PriorityTests, SerializeToInvalidStream) {
    std::filesystem::path path("./invalidStreamTest.txt");
    std::ofstream stream(path);
    stream.exceptions(std::ios_base::badbit);
    stream.close();
    EXPECT_ANY_THROW(stream << Priority::normal);
}