cmake_minimum_required(VERSION 3.14)

project(os.text-mode-driver)

add_subdirectory(source bin)
add_subdirectory(tests tests)

include(CTest)

enable_testing()

add_test(NAME kernel_tests COMMAND tests/kernel_tests)