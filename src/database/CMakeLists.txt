cmake_minimum_required(VERSION 3.0)

set(SOURCES
        Factory.cpp
        Gateway.cpp)

set(HEADERS
        ../../include/database/Gateway.h
        ../../include/database/Factory.h)

add_library(database ${SOURCES} ${HEADERS})
find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)
target_link_libraries(database Qt5::Core Qt5::Network Qt5::Sql)