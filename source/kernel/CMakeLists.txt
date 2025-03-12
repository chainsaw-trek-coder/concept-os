cmake_minimum_required(VERSION 3.14)

project(kernel)

include(ExternalProject)
ExternalProject_Add(kernel_common
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}/kernel-common"
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../kernel-common/source"
    INSTALL_COMMAND cmake -E echo "Skipping install step."
)

add_subdirectory(source bin)
add_subdirectory(tests tests)

include(CTest)
enable_testing()
add_test(NAME kernel_tests COMMAND tests/kernel_tests)