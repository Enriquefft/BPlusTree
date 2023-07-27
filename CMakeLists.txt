cmake_minimum_required(VERSION 3.26)

project(
  BPlusTree
  VERSION 1.0
  DESCRIPTION "Generic B+ Tree implementation in C++"
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(bplustree_test src/test.cpp)

target_include_directories(bplustree_test PRIVATE include)

target_compile_features(bplustree_test PRIVATE cxx_std_20)
