#include "stream.hpp"

namespace os
{
    enum bitness
    {
        _32bit = 0,
        _64bit = 1
    };

    enum endianess
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

    enum instruction_set
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
        bitness bitness;
        endianess endianess;
        elf_type type;
        instruction_set instruction_set;
        unsigned long long program_entry;

    public:
        elf_reader(stream &stream);
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
            this->bitness = static_cast<int>(header[4]);
            this->endianess = static_cast<int>(header[5]);
            this->type = *reinterpret_cast<short*>(&header[16]);
            this->instruction_set = *reinterpret_cast<short*>(&header[18]);

            if(this->bitness == bitness::_32bit)
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