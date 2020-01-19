#pragma once

#include <string>

namespace balancedbanana {
    namespace configfiles {

        //The priorities are used by the scheduler to sort jobs by their specified priority.
        enum class Priority {
            low = 1,//This priority can be used for jobs that are less important than those with the standard priority.
            normal = 2,//This priority can be used for most jobs that should be scheduled and should not take too much time.
            high = 3,//This priority can be used for Jobs that are important to finish quickly and are favoured by the scheduler.
            emergency = 4//This priority can be used for the most important jobs that have to be started instantly. The scheduler might even cancel a running job to let the ones with this priority take it's place.
        };

        std::string to_string(Priority priority);

        std::ostream &operator <<(std::ostream &, Priority);

    }
}