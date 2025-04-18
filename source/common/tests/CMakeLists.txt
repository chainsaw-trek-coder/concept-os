cmake_minimum_required(VERSION 3.14)

# project(kernel_tests)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

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

include_directories("../include")

FILE(GLOB C_SRCS *.cpp **/*.cpp ipc/*.cpp)

add_executable(
  common_lib_tests
  ${C_SRCS}
)

target_link_libraries(
  common_lib_tests
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(common_lib_tests)