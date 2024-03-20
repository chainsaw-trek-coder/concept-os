#include "memory/x86_32/gdt.hpp"
#include "interrupts/x86_32/idt.hpp"

struct cpu
{
    // Segmentation registers
    static void set_gdtr(global_descriptor_table<1> *gdt);
    static void set_cs(short segment_index, bool is_ldt, char priviledge_level);
    static void set_ss(short segment_index, bool is_ldt, char priviledge_level);
    static void set_ds(short segment_index, bool is_ldt, char priviledge_level);
    static void set_es(short segment_index, bool is_ldt, char priviledge_level);
    static void set_fs(short segment_index, bool is_ldt, char priviledge_level);
    static void set_gs(short segment_index, bool is_ldt, char priviledge_level);

    // Interrupts
    static void set_idtr(interrupt_descriptor_table *idt);

private:
    static short compute_selector(short segment_index, bool is_ldt, char priviledge_level);
};

inline void cpu::set_gdtr(global_descriptor_table<1> *gdt)
{
    asm volatile(
        "lgdt (%0)" : "=r"(gdt) :);
}

inline void cpu::set_cs(short segment_index, bool is_ldt, char priviledge_level)
{
    short selector = compute_selector(segment_index, is_ldt, priviledge_level);

    asm volatile(
        "mov %%cs, %0" : "=r"(selector) :);
}

inline void cpu::set_ss(short segment_index, bool is_ldt, char priviledge_level)
{
    short selector = compute_selector(segment_index, is_ldt, priviledge_level);

    asm volatile(
        "mov %%ss, %0" : "=r"(selector) :);
}

inline void cpu::set_ds(short segment_index, bool is_ldt, char priviledge_level)
{
    short selector = compute_selector(segment_index, is_ldt, priviledge_level);

    asm volatile(
        "mov %%ds, %0" : "=r"(selector) :);
}

inline void cpu::set_es(short segment_index, bool is_ldt, char priviledge_level)
{
    short selector = compute_selector(segment_index, is_ldt, priviledge_level);

    asm volatile(
        "mov %%es, %0" : "=r"(selector) :);
}

inline void cpu::set_fs(short segment_index, bool is_ldt, char priviledge_level)
{
    short selector = compute_selector(segment_index, is_ldt, priviledge_level);

    asm volatile(
        "mov %%fs, %0" : "=r"(selector) :);
}

inline void cpu::set_gs(short segment_index, bool is_ldt, char priviledge_level)
{
    short selector = compute_selector(segment_index, is_ldt, priviledge_level);

    asm volatile(
        "mov %%gs, %0" : "=r"(selector) :);
}

inline void cpu::set_idtr(interrupt_descriptor_table *idt)
{
    asm volatile(
        "lidt (%0)" : "=r"(idt) :);
}

inline short cpu::compute_selector(short segment_index, bool is_ldt, char priviledge_level)
{
    return (segment_index << 3) | (is_ldt ? 1 << 2 : 0) | (priviledge_level & 0x3);
}
