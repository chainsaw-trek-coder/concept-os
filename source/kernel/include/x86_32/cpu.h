#include "memory/x86_32/gdt.hpp"
#include "interrupts/x86_32/idt.hpp"

// TODO: Convert the assembly here into Intel syntax.
//       It's easier to read and matches Intel documentation.

struct cpu
{
    // Segmentation registers
    static void set_gdtr(flat_global_descriptor_table *gdt, unsigned short entry_count);
    static void set_cs(short segment_index /*, bool is_ldt, char priviledge_level */);
    static void set_ss(short segment_index, bool is_ldt, char priviledge_level);
    static void set_ds(short segment_index, bool is_ldt, char priviledge_level);
    static void set_es(short segment_index, bool is_ldt, char priviledge_level);
    static void set_fs(short segment_index, bool is_ldt, char priviledge_level);
    static void set_gs(short segment_index, bool is_ldt, char priviledge_level);

    // Interrupts
    static void set_idtr(interrupt_descriptor_table *idt, unsigned short entry_count);

    // Paging
    static void set_page_directory(page_directory *directory);
    static void enable_paging();

private:
    static short compute_selector(short segment_index, bool is_ldt, char priviledge_level);
};

inline void cpu::set_gdtr(flat_global_descriptor_table *gdt, unsigned short entry_count)
{
    struct __attribute__((packed)) gdt_record
    {
        unsigned short size;
        void *base_address;
    };

    gdt_record record;
    record.base_address = gdt;
    record.size = sizeof(segment_descriptor) * entry_count;

    asm volatile(
        "lgdt %0" : : "m"(record));
}

inline void cpu::set_cs(short segment_index /*, bool is_ldt, char priviledge_level */)
{
    // short selector = compute_selector(segment_index, is_ldt, priviledge_level);

    asm volatile(
        // "pushw %0\n"
        // "pushl $next\n"
        // "retf\n"
        "jmp $0x1, $next\n"
        "next:\n"
        "nop" : : "m"(segment_index) :);
}

inline void cpu::set_ss(short segment_index, bool is_ldt, char priviledge_level)
{
    short selector = compute_selector(segment_index, is_ldt, priviledge_level);

    asm volatile(
        "movw %0, %%ss" : : "r"(selector) :);
}

inline void cpu::set_ds(short segment_index, bool is_ldt, char priviledge_level)
{
    short selector = compute_selector(segment_index, is_ldt, priviledge_level);

    asm volatile(
        "movw %0, %%ds" : : "r"(selector) :);
}

inline void cpu::set_es(short segment_index, bool is_ldt, char priviledge_level)
{
    short selector = compute_selector(segment_index, is_ldt, priviledge_level);

    asm volatile(
        "movw %0, %%es" : : "r"(selector) :);
}

inline void cpu::set_fs(short segment_index, bool is_ldt, char priviledge_level)
{
    short selector = compute_selector(segment_index, is_ldt, priviledge_level);

    asm volatile(
        "movw %0, %%fs" : : "r"(selector) :);
}

inline void cpu::set_gs(short segment_index, bool is_ldt, char priviledge_level)
{
    short selector = compute_selector(segment_index, is_ldt, priviledge_level);

    asm volatile(
        "movw %0, %%gs" : : "r"(selector) :);
}

inline void cpu::set_idtr(interrupt_descriptor_table *idt, unsigned short entry_count)
{
    struct __attribute__((packed)) idt_record
    {
        unsigned short size;
        void *base_address;
    };

    idt_record record;
    record.base_address = idt;
    record.size = sizeof(interrupt_gate) * entry_count;

    asm volatile(
        "lidt %0" : : "m"(record));
}

inline void cpu::set_page_directory(page_directory *directory)
{
    asm volatile(
        "mov %0, %%cr3" : : "r"(directory));
}

inline void cpu::enable_paging()
{
    asm volatile(
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"
        "mov %%eax, %%cr0\n"
        :
        :);
}

inline short cpu::compute_selector(short segment_index, bool is_ldt, char priviledge_level)
{
    return (segment_index << 3) | (is_ldt ? 0x4 : 0) | (priviledge_level & 0x3);
}
