cmake_minimum_required(VERSION 3.14)

# project(kernel_common_tests)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
# SET(CMAKE_ASM-ATT_FLAGS "${CMAKE_ASM-ATT_FLAGS} --32")

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

add_compile_definitions(__testing__)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

FILE(GLOB C_SRCS *.cpp **/*.cpp ipc/*.cpp ../source/**/*.cpp)

add_executable(
  fs_lib_tests
  ${C_SRCS}
)

add_dependencies(fs_lib_tests fs_library_rust)

target_link_libraries(
  fs_lib_tests
  ${CMAKE_BINARY_DIR}/i686-unknown-linux-gnu/release/libconcept_fs.a
  GTest::gtest_main  
)

include(GoogleTest)
gtest_discover_tests(fs_lib_tests)
