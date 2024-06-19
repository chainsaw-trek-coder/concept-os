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

    class elf_reader
    {
    private:
        bool is_valid;
        elf_bitness bitness;
        elf_endianess endianess;
        elf_type type;
        elf_instruction_set instruction_set;
        unsigned long long program_entry;

    public:
        elf_reader(stream &stream);

        elf_bitness get_bitness() { return bitness; }
        elf_endianess get_endianess() { return endianess; }
        elf_type get_type() { return type; }
        elf_instruction_set get_instruction_set() { return instruction_set; }
    };

    inline elf_reader::elf_reader(stream &stream)
    {
        unsigned char header[64];
        
        is_valid = false;
        if(stream.read_bytes(bytes(header, 64), 64) == 64)
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
                this->program_entry = *reinterpret_cast<unsigned*>(&header[24]);
            }
            else
            {
                this->program_entry = *reinterpret_cast<unsigned long long*>(&header[24]);
            }
        }

    }
}