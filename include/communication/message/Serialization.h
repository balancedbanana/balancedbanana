#pragma once

#include <ostream>

namespace balancedbanana {
    namespace communication {
        namespace serialization {

            void insertString(std::ostream &stream, const std::string &value);

            template<typename T>
            void insert(std::ostream &stream, T value);

            std::string extractString(const char *data, size_t &iterator, size_t size);

            template<typename T>
            T extract(const char *data, size_t &iterator, size_t size);

        }
    }
}