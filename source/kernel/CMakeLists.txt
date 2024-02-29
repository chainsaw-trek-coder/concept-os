cmake_minimum_required(VERSION 3.14)

project(kernel)

add_subdirectory(source kernel)
add_subdirectory(tests tests)

include(CTest)

enable_testing()

add_test(NAME hello_test COMMAND tests/hello_test)