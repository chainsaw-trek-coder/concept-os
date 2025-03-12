CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

# project(kernel)

set(CMAKE_ASM-ATT_COMPILER /home/vscode/opt/cross/bin/i686-elf-as)
set(CMAKE_C_COMPILER   /home/vscode/opt/cross/bin/i686-elf-gcc)
set(CMAKE_CXX_COMPILER /home/vscode/opt/cross/bin/i686-elf-g++)
set(CMAKE_FIND_ROOT_PATH /home/vscode/opt/cross/bin/)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_CXX_FLAGS "-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti")
SET(CMAKE_EXE_LINKER_FLAGS "-ffreestanding -O2 -nostdlib")
SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

ENABLE_LANGUAGE(ASM-ATT)

configure_file(linker.ld ./ COPYONLY)

FILE(GLOB C_SRCS *.cpp **/*.cpp memory/*/*.cpp)

include_directories("../include", "../../common/include", "../../kernel-common/include")

ADD_EXECUTABLE(kernel_loader ${C_SRCS} x86_32/kernel.s x86_32/boot.s)
add_dependencies(kernel_loader kernel_common)

target_link_libraries(kernel_loader "${CMAKE_CURRENT_BINARY_DIR}/../kernel-common/src/kernel_common-build/libkernel_common.a")

SET_TARGET_PROPERTIES(kernel_loader PROPERTIES LINK_FLAGS "-T linker.ld -ffreestanding -O2 -nostdlib")