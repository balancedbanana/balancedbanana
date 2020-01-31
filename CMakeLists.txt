cmake_minimum_required(VERSION 3.0)

enable_language(CXX)

project(balancedbanana)

set(CMAKE_CXX_FLAGS "--coverage")

find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)
find_package(OpenSSL COMPONENTS Crypto REQUIRED)

include_directories(include)

add_subdirectory(src/client)
add_subdirectory(src/worker)
add_subdirectory(testsrc/worker)
add_subdirectory(src/communication)
add_subdirectory(src/configfiles)
add_subdirectory(testsrc/configfiles)
add_subdirectory(src/database)
add_subdirectory(src/scheduler)