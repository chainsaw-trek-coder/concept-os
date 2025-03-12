#include "stream.hpp"

namespace os
{
    enum elf_bitness
    {
        none = 0,
        _32bit = 1,
        _64bit = 2
    };

    enum elf_endianess
    {
        little_endian = 1,
        big_endian = 2
    };

    enum elf_type
    {
        relocatable = 1,
        executable = 2,
        shared = 3,
        core = 4
    };

    enum elf_instruction_set
    {
        non_specific = 0,
        spark = 2,
        x86 = 3,
        mips = 8,
        power_pc = 0x14,
        arm = 0x28,
        x86_64 = 0x3E,
        aarch64 = 0xB7,
        risk_v = 0xF3
    };

    struct elf_section
    {
        unsigned long long name = 0;
        unsigned long long type = 0;
        unsigned long long flags = 0;
        unsigned long long address_in_memory = 0;
        unsigned long long offset_in_file = 0;
        unsigned long long section_size = 0;
        unsigned long long link = 0;
        unsigned long long info = 0;
        unsigned long long address_alignment = 0;
        unsigned long long entry_size_if_applicable = 0;

        inline bool is_writable() { return 0x1 & flags; }
        inline bool is_allocated() { return 0x2 & flags; }
        inline bool is_executable() { return 0x4 & flags; }
    };

    class elf_reader
    {
    private:

        stream *elf_stream;

        bool is_valid;
        elf_bitness bitness;
        elf_endianess endianess;
        elf_type type;
        elf_instruction_set instruction_set;
        unsigned long long program_entry_offset;
        unsigned long long program_header_offset;
        unsigned long long section_header_offset;

        unsigned flags;
        short elf_header_size = 0;
        short program_header_entry_size = 0;
        short number_of_program_header_entries;
        short section_header_entry_size = 0;
        short number_of_section_header_entries = 0;

        short section_header_string_table_index;

        short current_section_index;
        elf_section current_section;

    public:
        elf_reader(stream &elf_stream);

        elf_bitness get_bitness() { return bitness; }
        elf_endianess get_endianess() { return endianess; }
        elf_type get_type() { return type; }
        elf_instruction_set get_instruction_set() { return instruction_set; }

        unsigned long long get_program_entry_offset() { return program_entry_offset; }
        unsigned long long get_program_header_offset() { return program_header_offset; }
        unsigned long long get_section_header_offset() { return section_header_offset; }

        unsigned get_flags() { return flags; }

        short get_elf_header_size() { return elf_header_size; }
        short get_program_header_entry_size() { return program_header_entry_size; }
        short get_number_of_program_header_entries() { return number_of_program_header_entries; }
        short get_section_header_entry_size() { return section_header_entry_size; }
        short get_number_of_section_header_entries() { return number_of_section_header_entries; }

        short get_section_header_string_table_index() { return section_header_string_table_index; }

        void set_current_section_index(short index);
        short get_current_section_index();
        elf_section &get_current_section();
    };

#define CAST_BINARY_FIELD(array, array_offset, type) *reinterpret_cast<type*>(&array[array_offset])

    inline elf_reader::elf_reader(stream &elf_stream)
    {
        this->elf_stream = &elf_stream;

        unsigned char header[64];
        
        is_valid = false;
        if(elf_stream.read_bytes(bytes(header, 64), 64) == 64)
        {
            is_valid = true;
            is_valid &= (header[0] == 0x7F);
            is_valid &= (header[1] == 'E');
            is_valid &= (header[2] == 'L');
            is_valid &= (header[3] == 'F');
        }

        if(is_valid)
        {
            this->bitness = static_cast<elf_bitness>(static_cast<int>(header[4]));
            this->endianess = static_cast<elf_endianess>(static_cast<int>(header[5]));
            this->type = static_cast<elf_type>(*reinterpret_cast<short*>(&header[16]));
            this->instruction_set = static_cast<elf_instruction_set>(*reinterpret_cast<short*>(&header[18]));

            if(this->bitness == elf_bitness::_32bit)
            {
                this->program_entry_offset = *reinterpret_cast<unsigned*>(&header[24]);
                this->program_header_offset = CAST_BINARY_FIELD(header, 28, unsigned);
                this->section_header_offset = CAST_BINARY_FIELD(header, 32, unsigned);
                this->flags = CAST_BINARY_FIELD(header, 36, unsigned);
                this->elf_header_size = CAST_BINARY_FIELD(header, 40, short);
                this->program_header_entry_size = CAST_BINARY_FIELD(header, 42, short);
                this->number_of_program_header_entries = CAST_BINARY_FIELD(header, 44, short);
                this->section_header_entry_size = CAST_BINARY_FIELD(header, 46, short);
                this->number_of_section_header_entries = CAST_BINARY_FIELD(header, 48, short);
                this->section_header_string_table_index = CAST_BINARY_FIELD(header, 50, short);
            }
            else
            {
                this->program_entry_offset = *reinterpret_cast<unsigned long long*>(&header[24]);
                this->program_header_offset = CAST_BINARY_FIELD(header, 32, unsigned long long);
                this->section_header_offset = CAST_BINARY_FIELD(header, 40, unsigned long long);
                this->flags = CAST_BINARY_FIELD(header, 48, unsigned);
                this->elf_header_size = CAST_BINARY_FIELD(header, 52, short);
                this->program_header_entry_size = CAST_BINARY_FIELD(header, 54, short);
                this->number_of_program_header_entries = CAST_BINARY_FIELD(header, 56, short);
                this->section_header_entry_size = CAST_BINARY_FIELD(header, 58, short);
                this->number_of_section_header_entries = CAST_BINARY_FIELD(header, 60, short);
                this->section_header_string_table_index = CAST_BINARY_FIELD(header, 62, short);
            }
        }

    }
    
    inline void elf_reader::set_current_section_index(short index)
    {
        unsigned char section[sizeof(elf_section)];

        elf_stream->seek(this->section_header_offset + (this->section_header_entry_size * index));

        if(this->bitness == elf_bitness::_32bit)
        {
            if(elf_stream->read_bytes(bytes(section, sizeof(elf_section)), sizeof(elf_section) / 2) == sizeof(elf_section) / 2)
            {
                current_section.name = CAST_BINARY_FIELD(section, 0, unsigned);
                current_section.type = CAST_BINARY_FIELD(section, 4, unsigned);
                current_section.flags = CAST_BINARY_FIELD(section, 8, unsigned);
                current_section.address_in_memory = CAST_BINARY_FIELD(section, 12, unsigned);
                current_section.offset_in_file = CAST_BINARY_FIELD(section, 16, unsigned);
                current_section.section_size = CAST_BINARY_FIELD(section, 20, unsigned);
                current_section.link = CAST_BINARY_FIELD(section, 24, unsigned);
                current_section.info = CAST_BINARY_FIELD(section, 28, unsigned);
                current_section.address_alignment = CAST_BINARY_FIELD(section, 32, unsigned);
                current_section.entry_size_if_applicable = CAST_BINARY_FIELD(section, 36, unsigned);
            }
            else
            {
                // TODO: We really need exception handling...
            }
        }
        else
        {
            // TODO: ...
        }

        this->current_section_index = index;
    }
    inline short elf_reader::get_current_section_index()
    {
        return this->current_section_index;
    }
    inline elf_section &elf_reader::get_current_section()
    {
        return this->current_section;
    }
}