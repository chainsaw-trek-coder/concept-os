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

TEST(ELFReaderTests, elf_reader_can_read_section)
{
    bytes kernel_file(kernel_x86_32, kernel_x86_32_size);
    memory_stream stream(kernel_file);
    elf_reader reader(stream);

    // String seection
    reader.set_current_section_index(reader.get_section_header_string_table_index());
    auto string_section = reader.get_current_section();
    const char* current_section_name;

    reader.set_current_section_index(0);

    auto section = reader.get_current_section();

    EXPECT_EQ(section.name, 0);
    EXPECT_EQ(section.type, 0);
    EXPECT_EQ(section.flags, 0);
    EXPECT_EQ(section.address_in_memory, 0);
    EXPECT_EQ(section.offset_in_file, 0);
    EXPECT_EQ(section.section_size, 0);
    EXPECT_EQ(section.link, 0);
    EXPECT_EQ(section.info, 0);
    EXPECT_EQ(section.address_alignment, 0);
    EXPECT_EQ(section.entry_size_if_applicable, 0);

    // This section likely to contain all the executable code.
    reader.set_current_section_index(1);
    current_section_name = reinterpret_cast<const char*>(kernel_file.get_data()) + string_section.offset_in_file + reader.get_current_section().name;

    auto section_2 = reader.get_current_section();

    EXPECT_EQ(section_2.name, 31); // .text
    EXPECT_EQ(section_2.type, 1); // PROGBITS
    EXPECT_EQ(section_2.flags, 6); // Alloc & Executable instructions
    EXPECT_EQ(section_2.address_in_memory, 0);
    EXPECT_EQ(section_2.offset_in_file, 64);
    EXPECT_EQ(section_2.section_size, 4327);
    EXPECT_EQ(section_2.link, 0);
    EXPECT_EQ(section_2.info, 0);
    EXPECT_EQ(section_2.address_alignment, 16);
    EXPECT_EQ(section_2.entry_size_if_applicable, 0);

    // This section appears to contain relocation data for the previous.
    reader.set_current_section_index(2);
    current_section_name = reinterpret_cast<const char*>(kernel_file.get_data()) + string_section.offset_in_file + reader.get_current_section().name;
    auto section_3 = reader.get_current_section();

    EXPECT_EQ(section_3.name, 27); // .rel.text
    EXPECT_EQ(section_3.type, 9); // REL
    EXPECT_EQ(section_3.flags, 64);
    EXPECT_EQ(section_3.address_in_memory, 0);
    EXPECT_EQ(section_3.offset_in_file, 68892);
    EXPECT_EQ(section_3.section_size, 1064);
    EXPECT_EQ(section_3.link, 26);
    EXPECT_EQ(section_3.info, 1);
    EXPECT_EQ(section_3.address_alignment, 4);
    EXPECT_EQ(section_3.entry_size_if_applicable, 8);

    reader.set_current_section_index(section_3.link); // Load the symbol table.
    current_section_name = reinterpret_cast<const char*>(kernel_file.get_data()) + string_section.offset_in_file + reader.get_current_section().name;
    auto section_symbol_table = reader.get_current_section();

    section_symbol_table = section_symbol_table;
}