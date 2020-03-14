cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
enable_language(CXX)

project(balancedbanana)

find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)
find_package(OpenSSL COMPONENTS Crypto REQUIRED)
find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

SET(COVERAGE OFF CACHE BOOL "Coverage")
SET(TESTING OFF CACHE BOOL "Enable Tests")
SET(ENABLE_ADDRESS_SANITIZER OFF CACHE BOOL "Enable Address Sanitizer")

if(ENABLE_ADDRESS_SANITIZER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -g -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g -fsanitize=address")
endif()

if(COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
  set(TESTING TRUE)
endif()
if(TESTING)
  enable_testing()
  find_package(GTest)
  set(GTEST GTest::GTest GTest::Main --coverage)
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
      set(GTEST gtest gtest_main --coverage)
  endif()
endif()

# Download and unpack Webserver at configure time
configure_file(contrib/Webserver/CMakeLists.txt.in Webserver-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Webserver-download )
if(result)
  message(FATAL_ERROR "CMake step for Webserver failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Webserver-download )
if(result)
  message(FATAL_ERROR "Build step for Webserver failed: ${result}")
endif()

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

if(UNIX)
  set(WHOLEARCHIVEBEG -Wl,--whole-archive)
  set(WHOLEARCHIVEEND -Wl,--no-whole-archive)
endif()

# Add Webserver directly to our build.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/Webserver-src
                 ${CMAKE_CURRENT_BINARY_DIR}/Webserver-build
                 EXCLUDE_FROM_ALL)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/Webserver-src)

include_directories(include)
include_directories(contrib)
include_directories(include/libs)

add_subdirectory(src/client)
add_subdirectory(src/commandLineInterface)
add_subdirectory(src/communication)
add_subdirectory(src/configfiles)
add_subdirectory(src/database)
add_subdirectory(src/scheduler)
add_subdirectory(src/timedevents)
add_subdirectory(src/worker)
if(TESTING)
  add_subdirectory(testsrc/commandLineInterface)
  add_subdirectory(testsrc/communication)
  add_subdirectory(testsrc/configfiles)
  add_subdirectory(testsrc/database)
  add_subdirectory(testsrc/scheduler)
  add_subdirectory(testsrc/timedevents)
  add_subdirectory(testsrc/worker)
  add_subdirectory(testsrc/integration)
endif()
