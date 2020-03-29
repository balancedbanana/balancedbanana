#include <gtest/gtest.h>
#include <database/JobStatus.h>

using namespace balancedbanana::database;

TEST(StatusToStringTest, StatusToStringTest_Scheduled_Test){
    EXPECT_EQ("scheduled", status_to_string(JobStatus::scheduled));
}

TEST(StatusToStringTest, StatusToStringTest_Processing_Test){
    EXPECT_EQ("processing", status_to_string(JobStatus::processing));
}

TEST(StatusToStringTest, StatusToStringTest_Paused_Test){
    EXPECT_EQ("paused", status_to_string(JobStatus::paused));
}

TEST(StatusToStringTest, StatusToStringTest_Interrupted_Test){
    EXPECT_EQ("interrupted", status_to_string(JobStatus::interrupted));
}

TEST(StatusToStringTest, StatusToStringTest_Finished_Test){
    EXPECT_EQ("finished", status_to_string(JobStatus::finished));
}

TEST(StatusToStringTest, StatusToStringTest_Canceled_Test){
    EXPECT_EQ("canceled", status_to_string(JobStatus::canceled));
}

TEST(StatusToStringTest, StatusToStringTest_Invalid_Test){
    EXPECT_THROW(status_to_string(static_cast<JobStatus>(10)), std::invalid_argument);
}

TEST(StringToStatusTest, StringToStatusTest_Scheduled_Test){
    EXPECT_EQ(JobStatus::scheduled, string_to_status("scheduled"));
}

TEST(StringToStatusTest, StringToStatusTest_Processing_Test){
    EXPECT_EQ(JobStatus::processing, string_to_status("processing"));
}

TEST(StringToStatusTest, StringToStatusTest_Paused_Test){
    EXPECT_EQ(JobStatus::paused, string_to_status("paused"));
}

TEST(StringToStatusTest, StringToStatusTest_Interrupted_Test){
    EXPECT_EQ(JobStatus::interrupted, string_to_status("interrupted"));
}

TEST(StringToStatusTest, StringToStatusTest_Finished_Test){
    EXPECT_EQ(JobStatus::finished, string_to_status("finished"));
}

TEST(StringToStatusTest, StringToStatusTest_Canceled_Test){
    EXPECT_EQ(JobStatus::canceled, string_to_status("canceled"));
}

TEST(StringToStatusTest, StringToStatusTest_Invalid_Test){
    EXPECT_THROW(string_to_status(""), std::invalid_argument);
}

