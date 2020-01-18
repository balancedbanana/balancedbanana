cmake_minimum_required(VERSION 3.0)

enable_language(CXX)

project(balancedbanana)

find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)
find_package(OpenSSL COMPONENTS Crypto REQUIRED)

include_directories(include)

add_subdirectory(src/client)
add_subdirectory(src/worker)
add_subdirectory(src/communication)
add_subdirectory(src/configfiles)
add_subdirectory(src/database)
add_subdirectory(src/scheduler)