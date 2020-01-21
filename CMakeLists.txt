cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
enable_language(CXX)

project(balancedbanana)

find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)
find_package(OpenSSL COMPONENTS Crypto REQUIRED)

include_directories(include)

add_subdirectory(src/client)
add_subdirectory(src/worker)
add_subdirectory(src/commandLineInterface)
add_subdirectory(testsrc/worker)
add_subdirectory(src/communication)
add_subdirectory(src/configfiles)
add_subdirectory(src/database)
add_subdirectory(src/scheduler)