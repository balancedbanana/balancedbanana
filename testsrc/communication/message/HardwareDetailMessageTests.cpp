#include <gtest/gtest.h>
#include <communication/message/HardwareDetailMessage.h>

using namespace balancedbanana::communication;

class HardwareDetailMessageTest : public testing::Test {
protected:
    HardwareDetailMessage *msg;
    char *serialized;
    size_t *size;

    HardwareDetailMessageTest() : msg(nullptr), serialized(nullptr), size(nullptr) {
    }

    void SetUp() override {
        msg = new HardwareDetailMessage(42, 654321, "my operating system");
        size = new size_t(4 * sizeof(uint32_t) + msg->GetOsIdentifier().size());
        serialized = new char[*size];
        *reinterpret_cast<uint32_t *>(serialized) = HARDWARE_DETAIL;
        *reinterpret_cast<uint32_t *>(serialized + sizeof(uint32_t)) = 42;
        *reinterpret_cast<uint32_t *>(serialized + 2 * sizeof(uint32_t)) = 654321;
        *reinterpret_cast<uint32_t *>(serialized + 3 * sizeof(uint32_t)) = msg->GetOsIdentifier().size();
        memcpy(serialized + 4 * sizeof(uint32_t), msg->GetOsIdentifier().data(), msg->GetOsIdentifier().size());
    }

    void TearDown() override {
        delete msg;
        delete[] serialized;
        delete size;
    }
};

TEST_F(HardwareDetailMessageTest, Getters) {
    EXPECT_EQ(HARDWARE_DETAIL, msg->GetType());
    EXPECT_EQ(42, msg->GetCoreCount());
    EXPECT_EQ(654321, msg->GetRamSize());
    EXPECT_EQ("my operating system", msg->GetOsIdentifier());
}

TEST_F(HardwareDetailMessageTest, Serialize) {
    std::string string = msg->serialize();
    EXPECT_EQ(*size, string.size());
    for(int i = 0; i < *size; i++) {
        EXPECT_EQ(serialized[i], string[i]);
    }
}

TEST_F(HardwareDetailMessageTest, Deserialize) {
    size_t iterator = 0;
    std::shared_ptr<Message> ptr = nullptr;
    EXPECT_NO_THROW(ptr = Message::deserialize(serialized, *size));
    EXPECT_TRUE(ptr);
    EXPECT_EQ(msg->GetType(), ptr->GetType());
    std::shared_ptr hdm = std::dynamic_pointer_cast<HardwareDetailMessage>(ptr);
    EXPECT_EQ(msg->GetCoreCount(), hdm->GetCoreCount());
    EXPECT_EQ(msg->GetRamSize(), hdm->GetRamSize());
    EXPECT_EQ(msg->GetOsIdentifier(), hdm->GetOsIdentifier());
}