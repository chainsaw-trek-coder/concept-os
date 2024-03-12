#if defined(__i386__)

#include "memory/x86_32/gdt.hpp"
extern unsigned global_mem_size;

#endif

#if defined(__x86_64__)

extern unsigned long long global_mem_size;

#endif

extern void *global_mem_start;
extern global_descriptor_table<1> gdt;