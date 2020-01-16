cmake_minimum_required(VERSION 3.0)

enable_language(CXX)

project(balancedbanana)

find_package(Qt5 COMPONENTS Core Network REQUIRED)
find_package(OpenSSL COMPONENTS Crypto REQUIRED)

add_subdirectory(communication)
add_subdirectory(ConfigFiles)