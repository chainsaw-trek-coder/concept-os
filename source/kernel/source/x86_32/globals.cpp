#include "x86_32/globals.hpp"
#include "x86_32/cpu.h"
#include "memory/utils.h"
#include "memory/paging.hpp"
#include "x86_32/globals.hpp"

// Intel recommends alignment on 8-byte boundary for best performance.
global_descriptor_table<1> __attribute__((aligned(8))) gdt;
void *global_mem_start = nullptr;
unsigned global_mem_size = 0;

interrupt_descriptor_table __attribute__((aligned(8))) idt;

unsigned end_of_kernel_variable __attribute__((section(".end_of_kernel")));
void *end_of_kernel = &end_of_kernel_variable;

page_directory *global_page_directory = nullptr;

void initialize_memory()
{
    // Setup gdt.
    auto &segment = gdt.segments[0];
    segment.set_base_address(nullptr);
    segment.clear_granularity_flag(); // Byte sizes
    segment.set_is_system(true);
    segment.set_limit(0xFFFFFFFF);
    segment.set_present(true);
    segment.set_priviledge_level(0);
    segment.set_type(segment_type::read_write_expand_down);

    // terminal_writestring("Setting up CPU...\n");
    cpu::set_gdtr(&gdt);

    // Setup registers.
    cpu::set_cs(1, false, 0);
    cpu::set_ds(1, false, 0);
    cpu::set_es(1, false, 0);
    cpu::set_fs(1, false, 0);
    cpu::set_gs(1, false, 0);
    cpu::set_ss(1, false, 0);

    // Setup paging.
    global_page_directory = new (get_aligned_address(end_of_kernel)) page_directory();
}