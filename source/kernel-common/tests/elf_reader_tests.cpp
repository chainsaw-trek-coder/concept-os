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

    EXPECT_EQ(reader.get_program_entry_offset(), 0);
    EXPECT_EQ(reader.get_program_header_offset(), 0);
    EXPECT_EQ(reader.get_section_header_offset(), 83916);

    EXPECT_EQ(reader.get_flags(), 0);
    EXPECT_EQ(reader.get_elf_header_size(), 52);
    EXPECT_EQ(reader.get_program_header_entry_size(), 0);
    EXPECT_EQ(reader.get_number_of_program_header_entries(), 0);

    EXPECT_EQ(reader.get_section_header_entry_size(), 40);
    EXPECT_EQ(reader.get_number_of_section_header_entries(), 29);

    EXPECT_EQ(reader.get_section_header_string_table_index(), 28);
}