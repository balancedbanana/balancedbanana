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
            }
        }

        Priority stopriority(std::string &value, bool &success) {
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