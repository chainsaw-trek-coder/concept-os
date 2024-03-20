#include "memory/x86_32/gdt.hpp"
#include "interrupts/x86_32/idt.hpp"

extern unsigned global_mem_size;
extern void *global_mem_start;
extern global_descriptor_table<1> gdt;

extern interrupt_descriptor_table idt;