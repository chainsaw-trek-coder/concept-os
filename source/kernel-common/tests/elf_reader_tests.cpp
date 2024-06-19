#include <gtest/gtest.h>
#include "test_globals.hpp"
#include "memory_stream.hpp"
#include "elf_reader.hpp"

using namespace os;

TEST(ELFReaderTests, elf_reader_can_read_header)
{
    bytes kernel_file(kernel_x86_32, kernel_x86_32_size);
    memory_stream stream(kernel_file);
    elf_reader reader(stream);

    EXPECT_EQ(reader.get_bitness(), elf_bitness::_32bit);
    EXPECT_EQ(reader.get_endianess(), elf_endianess::little_endian);
    EXPECT_EQ(reader.get_instruction_set(), elf_instruction_set::x86);
    EXPECT_EQ(reader.get_type(), elf_type::relocatable);
}