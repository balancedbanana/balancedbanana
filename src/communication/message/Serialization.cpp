#include <communication/message/Serialization.h>

namespace balancedbanana::communication::serialization {

    void insertString(std::ostream &stream, const std::string &value) {
        insert<uint32_t>(stream, value.size());
        stream.write(value.data(), value.size());
    }

    template<typename T>
    void insert(std::ostream &stream, T value) {
        stream.write(reinterpret_cast<const char *>(&value), sizeof(T));
    }

    std::string extractString(const char *data, size_t &iterator, size_t size) {
        if(data == nullptr) {
            throw std::invalid_argument("data must not be null");
        }
        uint32_t stringsize = extract<uint32_t>(data, iterator, size);
        if (iterator + stringsize > size) {
            throw std::invalid_argument("data is too small");
        }
        std::string value(data + iterator, stringsize);
        iterator += stringsize;
        return value;
    }

    template<typename T>
    T extract(const char *data, size_t &iterator, size_t size) {
        if(data == nullptr) {
            throw std::invalid_argument("data must not be null");
        }
        if (iterator + sizeof(T) > size) {
            throw std::invalid_argument("data is too small");
        }
        T value = *reinterpret_cast<const T *>(data + iterator);
        iterator += sizeof(T);
        return value;
    }

    template void insert(std::ostream &, uint16_t value);
    template void insert(std::ostream &, uint32_t value);
    template void insert(std::ostream &, uint64_t value);
    template void insert(std::ostream &, bool value);

    template uint16_t extract(const char *, size_t &, size_t);
    template uint32_t extract(const char *, size_t &, size_t);
    template uint64_t extract(const char *, size_t &, size_t);
    template bool extract(const char *, size_t &, size_t);

}