#include <gtest/gtest.h>
#include <configfiles/JobConfig.h>
#include <optional>
#include <fstream>

using namespace balancedbanana::configfiles;

void testEqual(JobConfig *expected, JobConfig *value) {
    EXPECT_EQ(expected->min_ram(), value->min_ram());
    EXPECT_EQ(expected->max_ram(), value->max_ram());
    EXPECT_EQ(expected->min_cpu_count(), value->min_cpu_count());
    EXPECT_EQ(expected->max_cpu_count(), value->max_cpu_count());
    EXPECT_EQ(expected->blocking_mode(), value->blocking_mode());
    EXPECT_EQ(expected->email(), value->email());
    EXPECT_EQ(expected->priority(), value->priority());
    EXPECT_EQ(expected->image(), value->image());
    EXPECT_EQ(expected->environment(), value->environment());
    EXPECT_EQ(expected->interruptible(), value->interruptible());
    EXPECT_EQ(expected->current_working_dir(), value->current_working_dir());
}

class JobConfigGetterSetterTests : public testing::Test {
protected:
    JobConfig *conf;

public:
    JobConfigGetterSetterTests() : conf(nullptr) {
    };

    void SetUp() override {
        conf = new JobConfig();
    }

    void TearDown() override {
        delete conf;
    }

};

TEST_F(JobConfigGetterSetterTests, MinRam) {
    EXPECT_EQ(std::nullopt, conf->min_ram());
    conf->set_min_ram(654321);
    EXPECT_EQ(654321, conf->min_ram());
    conf->set_min_ram(std::nullopt);
    EXPECT_NE(654321, conf->min_ram());
}

TEST_F(JobConfigGetterSetterTests, MaxRam) {
    EXPECT_EQ(std::nullopt, conf->max_ram());
    conf->set_max_ram(123456);
    EXPECT_EQ(123456, conf->max_ram());
    conf->set_max_ram(std::nullopt);
    EXPECT_NE(123456, conf->max_ram());
}

TEST_F(JobConfigGetterSetterTests, MinCpuCount) {
    EXPECT_EQ(std::nullopt, conf->min_cpu_count());
    conf->set_min_cpu_count(43);
    EXPECT_EQ(43, conf->min_cpu_count());
    conf->set_min_cpu_count(std::nullopt);
    EXPECT_NE(43, conf->min_cpu_count());
}

TEST_F(JobConfigGetterSetterTests, MaxCpuCount) {
    EXPECT_EQ(std::nullopt, conf->max_cpu_count());
    conf->set_max_cpu_count(42);
    EXPECT_EQ(42, conf->max_cpu_count());
    conf->set_max_cpu_count(std::nullopt);
    EXPECT_NE(42, conf->max_cpu_count());
}

TEST_F(JobConfigGetterSetterTests, BlockingMode) {
    EXPECT_EQ(std::nullopt, conf->blocking_mode());
    conf->set_blocking_mode(true);
    EXPECT_EQ(true, conf->blocking_mode());
    conf->set_blocking_mode(std::nullopt);
    EXPECT_NE(true, conf->blocking_mode());
}

TEST_F(JobConfigGetterSetterTests, EMail) {
    EXPECT_EQ("", conf->email());
    conf->set_email("mail@test.com");
    EXPECT_EQ("mail@test.com", conf->email());
    conf->set_email("");
    EXPECT_NE("mail@test.com", conf->email());
}

TEST_F(JobConfigGetterSetterTests, Priority) {
    EXPECT_EQ(std::nullopt, conf->priority());
    conf->set_priority(Priority::high);
    EXPECT_EQ(Priority::high, conf->priority());
    conf->set_priority(std::nullopt);
    EXPECT_NE(Priority::high, conf->priority());
}

TEST_F(JobConfigGetterSetterTests, Image) {
    EXPECT_EQ("", conf->image());
    conf->set_image("testimage");
    EXPECT_EQ("testimage", conf->image());
    conf->set_image("");
    EXPECT_NE("testimage", conf->image());
}

TEST_F(JobConfigGetterSetterTests, Environment) {
    std::vector<std::string> vector = {"str1", "str2", "str3", "str4"};
    EXPECT_EQ(std::nullopt, conf->environment());
    conf->set_environment(vector);
    EXPECT_EQ(vector.size(), conf->environment()->size());
    for(int i = 0; i < vector.size(); i++) {
        EXPECT_EQ(vector[i], conf->environment()->at(i));
    }
    conf->set_environment(std::nullopt);
    EXPECT_EQ(std::nullopt, conf->environment());
}

TEST_F(JobConfigGetterSetterTests, Interruptible) {
    EXPECT_EQ(std::nullopt, conf->interruptible());
    conf->set_interruptible(false);
    EXPECT_EQ(false, conf->interruptible());
    conf->set_interruptible(std::nullopt);
    EXPECT_NE(false, conf->interruptible());
}

TEST_F(JobConfigGetterSetterTests, CurrentWorkingDir) {
    std::filesystem::path currentDir(".");
    EXPECT_EQ("", conf->current_working_dir());
    conf->set_current_working_dir(currentDir);
    EXPECT_EQ(currentDir, conf->current_working_dir());
    conf->set_current_working_dir("");
    EXPECT_NE(currentDir, conf->current_working_dir());
}

class JobConfigSerializationTest : public testing::Test {
protected:
    JobConfig *config;
    std::string *serializedConfig;

    JobConfigSerializationTest() : config(nullptr), serializedConfig(nullptr) {
    }

    void SetUp() override {
        config = new JobConfig();
        config->set_min_ram(123456);
        config->set_max_ram(654321);
        config->set_min_cpu_count(42);
        config->set_max_cpu_count(43);
        config->set_blocking_mode(true);
        config->set_email("mail@test.com");
        config->set_priority(Priority::low);
        config->set_image("testimage");
        config->set_environment(std::vector<std::string>{"str1", "str2", "str3"});
        config->set_interruptible(false);
        config->set_current_working_dir(".");
        serializedConfig = new std::string(
                "min_ram:123456\n"
                "max_ram:654321\n"
                "min_cpu_count:42\n"
                "max_cpu_count:43\n"
                "blocking_mode:1\n"
                "email:mail@test.com\n"
                "priority:low\n"
                "image:testimage\n"
                "environment:[\n"
                "str1\n"
                "str2\n"
                "str3\n"
                "]\n"
                "interruptible:0\n"
                "current_working_dir:.\n"
                );
    }

    void TearDown() override {
        delete config;
        delete serializedConfig;
    }

};

TEST_F(JobConfigSerializationTest, Serialize) {
    std::stringstream serializer;
    config->Serialize(serializer);
    std::string serialized = serializer.str();
    EXPECT_EQ(*serializedConfig, serialized);
}

TEST_F(JobConfigSerializationTest, Deserialize) {
    std::stringstream deserializer(*serializedConfig);
    JobConfig c(deserializer);
    testEqual(config, &c);
}

TEST_F(JobConfigSerializationTest, Save) {
    std::filesystem::path path("./save-test.txt");
    EXPECT_EQ(true, config->Save(path));
    std::stringstream saved;
    std::ifstream stream(path);
    saved << stream.rdbuf();
    stream.close();
    EXPECT_EQ(*serializedConfig, saved.str());
}

TEST_F(JobConfigSerializationTest, Load) {
    std::filesystem::path path("./load-test-txt");
    std::ofstream stream(path);
    config->Serialize(stream);
    stream.close();
    JobConfig loaded(path);
    testEqual(config, &loaded);
}

TEST_F(JobConfigSerializationTest, LoadCriticalValues) {
    std::stringstream s("min_ram:\n"
                        "max_ram:1000000000000000000000000000000000000000000000000000000000000000000000000000000000\n"
                        "min_cpu_count:invalid\n"
                        "max_cpu_count:1000000000000000000000000000000000000000000000000\n"
                        "blocking_mode:maybe\n");
    JobConfig critical(s);
    EXPECT_EQ(std::nullopt, critical.min_ram());
    EXPECT_EQ(UINT64_MAX, critical.max_ram().value());
    EXPECT_EQ(std::nullopt, critical.min_cpu_count());
    EXPECT_EQ(UINT32_MAX, critical.max_cpu_count().value());
    EXPECT_EQ(std::nullopt, critical.blocking_mode());
}

class JobConfigMergeTest : public testing::Test {
protected:
    JobConfig *a, *b, *c;
    JobConfigMergeTest() : a(nullptr), b(nullptr), c(nullptr) {
    }

    void SetUp() override {
        std::stringstream astream("min_ram:123456\n"
                            "max_ram:654321\n"
                            "min_cpu_count:42\n"
                            "max_cpu_count:43\n"
                            "blocking_mode:1\n"
                            "email:mail@test.com\n"
                            "priority:low\n"
                            "image:testimage\n"
                            "environment:[\n"
                            "str1\n"
                            "str2\n"
                            "str3\n"
                            "]\n"
                            "interruptible:0\n"
                            "current_working_dir:.\n");
        std::stringstream bstream("min_ram:23\n"
                            "max_cpu_count:2\n"
                            "email:mail2@test.com\n"
                            "environment:[\n"
                            "str1\n"
                            "str3\n"
                            "]\n"
                            "current_working_dir:.\n");
        std::stringstream cstream("max_ram:34\n"
                            "min_cpu_count:1\n"
                            "blocking_mode:0\n"
                            "priority:high\n"
                            "image:testimage2\n"
                            "interruptible:0\n");
        a = new JobConfig(astream);
        b = new JobConfig(bstream);
        c = new JobConfig(cstream);
    }

    void TearDown() override {
        delete a;
        delete b;
        delete c;
    }

};

TEST_F(JobConfigMergeTest, MergeFullWithIncomplete) {
    JobConfig copyA = *a;
    copyA.Merge(*b);
    testEqual(a, &copyA);
}

TEST_F(JobConfigMergeTest, MergeIncompleteWithFull) {
    c->Merge(*a);
    std::stringstream s("min_ram:123456\n"
                        "max_ram:34\n"
                        "min_cpu_count:1\n"
                        "max_cpu_count:43\n"
                        "blocking_mode:0\n"
                        "email:mail@test.com\n"
                        "priority:high\n"
                        "image:testimage2\n"
                        "environment:[\n"
                        "str1\n"
                        "str2\n"
                        "str3\n"
                        "]\n"
                        "interruptible:0\n"
                        "current_working_dir:.\n"
                        );
    JobConfig expected(s);
    testEqual(&expected, c);
}

TEST_F(JobConfigMergeTest, MergeIncompleteWithIncomplete) {
    b->Merge(*c);
    std::stringstream s("min_ram:23\n"
                        "max_cpu_count:2\n"
                        "email:mail2@test.com\n"
                        "environment:[\n"
                        "str1\n"
                        "str3\n"
                        "]\n"
                        "current_working_dir:.\n"
                        "max_ram:34\n"
                        "min_cpu_count:1\n"
                        "blocking_mode:0\n"
                        "priority:high\n"
                        "image:testimage2\n"
                        "interruptible:0\n");
    JobConfig expected(s);
    testEqual(&expected, b);
}

TEST_F(JobConfigMergeTest, CopyJobConfig) {
    JobConfig bcopied(*b);
    testEqual(&bcopied, b);
    JobConfig ccopied = *c;
    testEqual(&ccopied, c);
}