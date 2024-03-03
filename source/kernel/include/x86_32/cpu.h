#include "memory/x86_32/gdt.hpp"

struct cpu
{
    static void set_gdtr(global_descriptor_table<1> *gdt);
    static void set_cs(short segment_index, bool is_ldt, char priviledge_level);
    static void set_ss(short segment_index, bool is_ldt, char priviledge_level);
    static void set_ds(short segment_index, bool is_ldt, char priviledge_level);
    static void set_es(short segment_index, bool is_ldt, char priviledge_level);
    static void set_fs(short segment_index, bool is_ldt, char priviledge_level);
    static void set_gs(short segment_index, bool is_ldt, char priviledge_level);
};

inline void cpu::set_gdtr(global_descriptor_table<1> *gdt)
{
    asm volatile(
        "lgdt (%0)" : "=r"(gdt) :
    );
}

inline void cpu::set_cs(short segment_index, bool is_ldt, char priviledge_level)
{
}

inline void cpu::set_ss(short segment_index, bool is_ldt, char priviledge_level)
{
}

inline void cpu::set_ds(short segment_index, bool is_ldt, char priviledge_level)
{
}

inline void cpu::set_es(short segment_index, bool is_ldt, char priviledge_level)
{
}

inline void cpu::set_fs(short segment_index, bool is_ldt, char priviledge_level)
{
}

inline void cpu::set_gs(short segment_index, bool is_ldt, char priviledge_level)
{
}
