#ifndef header_10cee31f_3a8b4df4_a08e_524f598ccc62
#define header_10cee31f_3a8b4df4_a08e_524f598ccc62

#include "os_new.hpp"
#include "memory/x86_32/gdt.hpp"
#include "interrupts/x86_32/idt.hpp"
#include "memory/paging.hpp"
#include "memory/mem_allocator.hpp"

extern unsigned global_mem_size;
extern void *global_mem_start;

extern "C" void *end_of_kernel_loader;

extern page_directory *kernel_page_directory;
extern mem_allocator memory;

// extern "C" unsigned char kernel_x86_32[];
// extern "C" int kernel_x86_32_size;

void initialize_memory();
void load_kernel();

#endif