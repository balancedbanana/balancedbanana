set(SOURCES
        Factory.cpp
        WorkerGateway.cpp
        UserGateway.cpp
        JobGateway.cpp
        Repository.cpp
        IGateway.cpp)

set(HEADERS
        ../../include/database/Factory.h
        ../../include/database/WorkerGateway.h
        ../../include/database/UserGateway.h
        ../../include/database/JobGateway.h
        ../../include/database/Repository.h
        ../../include/database/IGateway.h)

add_library(database ${SOURCES} ${HEADERS})
target_link_libraries(database PUBLIC Qt5::Core Qt5::Network Qt5::Sql configfiles)