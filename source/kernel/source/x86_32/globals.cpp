#include "x86_32/globals.hpp"

// Intel recommends alignment on 8-byte boundary for best performance.
global_descriptor_table<1> __attribute__((aligned(8))) gdt;
void* global_mem_start = nullptr;
unsigned global_mem_size = 0;

interrupt_descriptor_table __attribute__((aligned(8))) idt;

unsigned end_of_kernel_variable __attribute__((section(".end_of_kernel")));
void *end_of_kernel = &end_of_kernel_variable;