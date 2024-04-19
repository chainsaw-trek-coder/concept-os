cmake_minimum_required(VERSION 3.14)

project(common)

add_subdirectory(tests tests)

include(CTest)

enable_testing()

add_test(NAME common_lib_tests COMMAND tests/common_lib_tests)