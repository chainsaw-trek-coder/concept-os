#include "memory/x86_32/gdt.hpp"

struct cpu
{
    static void set_cs(segment_descriptor *segment);
    static void set_ss(segment_descriptor *segment);
    static void set_ds(segment_descriptor *segment);
    static void set_es(segment_descriptor *segment);
    static void set_fs(segment_descriptor *segment);
    static void set_gs(segment_descriptor *segment);
};

inline void
cpu::set_cs(segment_descriptor *segment)
{
    asm volatile(
        "movl %0, %%cs" : "=r"(segment) :);
}

inline void cpu::set_ss(segment_descriptor *segment)
{
    asm volatile(
        "movl %0, %%ss" : "=r"(segment) :);
}

inline void cpu::set_ds(segment_descriptor *segment)
{
    asm volatile(
        "movl %0, %%ds" : "=r"(segment) :);
}

inline void cpu::set_es(segment_descriptor *segment)
{
    asm volatile(
        "movl %0, %%es" : "=r"(segment) :);
}

inline void cpu::set_fs(segment_descriptor *segment)
{
    asm volatile(
        "movl %0, %%fs" : "=r"(segment) :);
}

inline void cpu::set_gs(segment_descriptor *segment)
{
    asm volatile(
        "movl %0, %%gs" : "=r"(segment) :);
}
