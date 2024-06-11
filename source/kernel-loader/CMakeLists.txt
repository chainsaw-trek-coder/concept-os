cmake_minimum_required(VERSION 3.14)

project(kernel_loader)

add_subdirectory(source bin)
add_subdirectory(tests tests)

include(CTest)

enable_testing()

add_test(NAME loader_tests COMMAND tests/loader_tests)