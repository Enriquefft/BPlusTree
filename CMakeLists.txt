cmake_minimum_required(VERSION 3.26)

project(
  BPlusTree
  VERSION 1.0
  DESCRIPTION "Generic B+ Tree implementation in C++"
  LANGUAGES CXX)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(CMAKE_EXPORT_COMPILE_COMMANDS "Export compile commands" ON)

# option(PACKAGE_TESTS "Build the tests" ON)

add_executable(bplustree_test src/test.cpp)

target_include_directories(bplustree_test PRIVATE include)

target_compile_features(bplustree_test PRIVATE cxx_std_20)

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

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()
