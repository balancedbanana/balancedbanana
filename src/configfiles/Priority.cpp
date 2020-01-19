//
// Created by niklas on 1/19/20.
//

#include <configfiles/Priority.h>

namespace balancedbanana::configfiles {

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

    std::ostream &operator<<(std::ostream &stream, Priority priority) {
        stream << to_string(priority);
        return stream;
    }

}