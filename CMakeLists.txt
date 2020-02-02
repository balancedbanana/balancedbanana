cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
enable_language(CXX)

project(balancedbanana)

set(CMAKE_CXX_FLAGS "--coverage")

find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)
find_package(OpenSSL COMPONENTS Crypto REQUIRED)
find_package(Threads REQUIRED)
link_libraries(Threads::Threads)
enable_testing()
find_package(GTest)
set(GTEST GTest::GTest GTest::Main)
if (NOT GTest_FOUND)
# Download and unpack googletest at configure time
configure_file(contrib/GTest/CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()
set(GTEST gtest gtest_main)
endif()

include_directories(include)
include_directories(contrib)

add_subdirectory(src/client)
add_subdirectory(src/worker)
add_subdirectory(src/commandLineInterface)
add_subdirectory(testsrc/worker)
add_subdirectory(src/communication)
add_subdirectory(src/configfiles)
add_subdirectory(testsrc/configfiles)
add_subdirectory(src/database)
add_subdirectory(src/scheduler)
add_subdirectory(testsrc/commandLineInterface)