#include <gtest/gtest.h>
#include <configfiles/ApplicationConfig.h>
#include <fstream>

using namespace balancedbanana::configfiles;

bool contains(const std::vector<std::string> &vec, const std::string &line) {
    for(const std::string &entry : vec) {
        if(entry == line) {
            return true;
        }
    }
    return false;
}

void testEqual(const ApplicationConfig *expected, const ApplicationConfig *value) {
    EXPECT_EQ(expected->size(), value->size());
    for(const auto &pair : *expected) {
        EXPECT_EQ(pair.second, value->at(pair.first));
    }
}

void testEqual(const std::string *expected, const std::string *value) {
    EXPECT_EQ(expected->size(), value->size());
    std::stringstream exp(*expected);
    std::stringstream val(*value);
    std::string line;
    std::vector<std::string> expectedlines;
    while(std::getline(exp, line)) {
        expectedlines.push_back(line);
    }
    while(std::getline(val, line)) {
        EXPECT_TRUE(contains(expectedlines, line));
    }
}

class ApplicationConfigTest : public testing::Test {
protected:

    ApplicationConfig *config;
    std::string *serialized;

    ApplicationConfigTest() : config(nullptr), serialized(nullptr) {
    }

    void SetUp() override {
        config = new ApplicationConfig();
        (*config)["name1"] = "value1";
        (*config)["name2"] = "value2";
        (*config)["name3"] = "value3";
        (*config)["name4"] = "value4";
        serialized = new std::string("name1:value1\n"
                                     "name2:value2\n"
                                     "name3:value3\n"
                                     "name4:value4\n");
    }

    void TearDown() override {
        delete config;
        delete serialized;
    }

};

TEST_F(ApplicationConfigTest, SerailizeToString) {
    std::stringstream stream;
    config->Serialize(stream);
    std::string str = stream.str();
    testEqual(serialized, &str);
}

TEST_F(ApplicationConfigTest, SerializeToFile) {
    std::filesystem::path path("./serializationTest.txt");
    EXPECT_TRUE(config->Save(path));
    std::ifstream stream(path);
    std::stringstream saved;
    saved << stream.rdbuf();
    std::string str = saved.str();
    testEqual(serialized, &str);
}

TEST_F(ApplicationConfigTest, DeserializeFromString) {
    std::stringstream stream(*serialized);
    ApplicationConfig deserialized(stream);
    testEqual(config, &deserialized);
}

TEST_F(ApplicationConfigTest, DeserializeFromFile) {
    std::filesystem::path path("./deserializationTest.txt");
    std::ofstream stream(path);
    stream << *serialized;
    stream.flush();
    stream.close();
    ApplicationConfig loaded(path);
    testEqual(config, &loaded);
}