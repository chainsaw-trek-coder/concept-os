cmake_minimum_required(VERSION 3.14)

project(kernel_common)

add_subdirectory(source source)
add_subdirectory(tests tests)

include(CTest)

enable_testing()

add_test(NAME kernel_common_lib_tests COMMAND tests/kernel_common_lib_tests)