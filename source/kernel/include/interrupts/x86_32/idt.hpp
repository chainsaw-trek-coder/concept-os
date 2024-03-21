#ifndef header_98643e76_ff38_4b4d_a2c1_90b1b7e64b23
#define header_98643e76_ff38_4b4d_a2c1_90b1b7e64b23

typedef void (*interrupt_handler)(void);

struct interrupt_gate
{
    unsigned dword1;
    unsigned dword2;

    interrupt_gate();

    void set_code_segment(short code_segment); // Should be ONE as we are using protected flat model.
    void set_entry_point(interrupt_handler address);
    void set_present(bool is_present); // Sets whether or not the segment is currently in memory.
    void set_priviledge_level(unsigned char priviledge);
    void set_32_bit_gate(bool is_32_bit);
};

struct interrupt_descriptor_table
{
    interrupt_gate gates[256];
};

inline interrupt_gate::interrupt_gate()
{
    dword1 = 0;
    dword2 = 0;
}

inline void interrupt_gate::set_code_segment(short code_segment)
{
    // Clear section.
    dword2 &= 0x0000FFFF;

    // Set value.
    dword2 |= (((unsigned)code_segment) << 16);
}

inline void interrupt_gate::set_entry_point(interrupt_handler address)
{
    dword1 &= 0x0000FFFF;
    dword1 |= (reinterpret_cast<unsigned>(address) & 0xFFFF0000);

    dword2 &= 0xFFFF0000;
    dword2 |= (reinterpret_cast<unsigned>(address) & 0x0000FFFF);
}

inline void interrupt_gate::set_present(bool is_present)
{
    auto mask = (unsigned)0x1 << 15;

    if(is_present)
        dword1 |= mask;
    else
        dword1 &= ~mask;
}

inline void interrupt_gate::set_priviledge_level(unsigned char priviledge)
{
    auto mask = 0x3 << 13;

    dword1 &= ~mask;
    dword1 |= ((priviledge << 13) & mask);
}

inline void interrupt_gate::set_32_bit_gate(bool is_32_bit)
{
    auto mask = 0x1 << 11;

    if(is_32_bit)
        dword1 |= mask;
    else
        dword1 &= ~mask;
}

#endif