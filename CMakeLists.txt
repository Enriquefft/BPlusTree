cmake_minimum_required(VERSION 3.26)

project(
  BPlusTree
  VERSION 1.0
  DESCRIPTION "Generic B+ Tree implementation in C++"
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

add_executable(bplus_tree src/main.cpp)

target_include_directories(bplus_tree PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_compile_features(bplus_tree PRIVATE cxx_std_20)

target_compile_options(bplus_tree PRIVATE -Wall -Wextra -Wpedantic)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git # Always fetch the
                                                          # latest version
  GIT_TAG main)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

if(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
  option(PACKAGE_TESTS "Build the tests" ON)
endif()

if(PACKAGE_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()
