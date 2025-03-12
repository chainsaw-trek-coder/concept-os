cmake_minimum_required(VERSION 3.14)

project(kernel_loader)

include(ExternalProject)
ExternalProject_Add(kernel_common
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}/kernel-common"
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../kernel-common/source"
    INSTALL_COMMAND cmake -E echo "Skipping install step."
)

# TODO: Move this binary into a file system
# ExternalProject_Add(kernel
#     PREFIX "${CMAKE_CURRENT_BINARY_DIR}/kernel"
#     SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../kernel/source"
#     INSTALL_COMMAND cmake -E echo "Skipping install step."
# )

add_subdirectory(source bin)
add_subdirectory(tests tests)

include(CTest)
enable_testing()
add_test(NAME loader_tests COMMAND tests/loader_tests)