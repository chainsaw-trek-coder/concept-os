#ifndef header_10cee31f_3a8b4df4_a08e_524f598ccc62
#define header_10cee31f_3a8b4df4_a08e_524f598ccc62

#include "memory/x86_32/gdt.hpp"
#include "interrupts/x86_32/idt.hpp"
#include "memory/paging.hpp"
#include "memory/memory_blocks.hpp"

extern unsigned global_mem_size;
extern void *global_mem_start;

extern "C" void *end_of_kernel_loader;

extern page_directory *kernel_page_directory;
extern memory_blocks memory;

extern "C" unsigned char kernel_x86_32[];
extern "C" int kernel_x86_32_size;

// Support placement new.
inline void *operator new(size_t, void *__p)
{
    return __p;
}

void initialize_memory();

#endif