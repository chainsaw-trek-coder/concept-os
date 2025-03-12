cmake_minimum_required(VERSION 3.14)

project(fs_library)

add_subdirectory(library)
add_subdirectory(tests)

include(CTest)
enable_testing()
add_test(NAME fs_lib_tests COMMAND tests/fs_lib_tests)