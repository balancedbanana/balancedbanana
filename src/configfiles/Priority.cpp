//
// Created by niklas on 1/19/20.
//

#include <configfiles/Priority.h>
#include <ostream>

namespace balancedbanana {

    namespace configfiles {

        std::string to_string(Priority priority) {
            switch (priority) {
                case Priority::low:
                    return "low";
                case Priority::normal:
                    return "normal";
                case Priority::high:
                    return "high";
                case Priority::emergency:
                    return "emergency";
                default:
                    throw std::invalid_argument("invalid priority");
            }
        }

        Priority from_string(std::string priority){
            if (priority == "low" || priority == "1")
                return Priority::low;

            else if (priority == "normal" || priority == "2")
                return Priority::normal;

            else if (priority == "high" || priority == "3")
                return Priority::high;

            else if (priority == "emergency" || priority == "4")
                return Priority::emergency;

            else
                throw std::invalid_argument("invalid priority");
        }

        Priority stopriority(const std::string &value, bool &success) {
            success = true;
            if (value == "low" || value == "1") {
                return Priority::low;
            } else if (value == "normal" || value == "2") {
                return Priority::normal;
            } else if (value == "high" || value == "3") {
                return Priority::high;
            } else if (value == "emergency" || value == "4") {
                return Priority::emergency;
            } else {
                success = false;
                return Priority::normal;
            }
        }

        std::ostream &operator<<(std::ostream &stream, Priority priority) {
            stream << to_string(priority);
            return stream;
        }

    }

}