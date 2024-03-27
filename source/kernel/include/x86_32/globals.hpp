#ifndef header_10cee31f_3a8b4df4_a08e_524f598ccc62
#define header_10cee31f_3a8b4df4_a08e_524f598ccc62

#include "memory/x86_32/gdt.hpp"
#include "interrupts/x86_32/idt.hpp"
#include "memory/paging.hpp"

extern unsigned global_mem_size;
extern void *global_mem_start;
extern flat_global_descriptor_table gdt;

extern interrupt_descriptor_table idt;

extern "C" void *end_of_kernel;

extern page_directory *global_page_directory;

// Support placement new.
inline void *operator new(size_t, void *__p)
{
    return __p;
}

void initialize_memory();

#endif