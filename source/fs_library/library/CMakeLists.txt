cmake_minimum_required(VERSION 3.14)

project (fs_library)

find_program(CARGO_EXECUTABLE cargo REQUIRED PATHS ~/.cargo/bin)

# include(ExternalProject)

add_custom_command(
    OUTPUT release/libconcept_fs.a
    COMMAND "${CARGO_EXECUTABLE}"  build --release --target=i686-unknown-linux-gnu --target-dir ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building Rust library"
    VERBATIM
)

add_custom_target(fs_library_rust DEPENDS release/libconcept_fs.a)

