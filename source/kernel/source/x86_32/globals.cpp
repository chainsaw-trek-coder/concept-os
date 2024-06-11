#include "x86_32/globals.hpp"
#include "x86_32/cpu.h"
#include "memory/utils.h"
#include "x86_32/globals.hpp"

// Intel recommends alignment on 8-byte boundary for best performance.
flat_global_descriptor_table __attribute__((aligned(8))) gdt;
interrupt_descriptor_table __attribute__((aligned(8))) idt;

unsigned global_mem_size;
void *global_mem_start;

memory_blocks memory;
page_directory *kernel_page_directory = nullptr;

void initialize_segmentation()
{
    gdt.initialize();

    cpu::set_gdtr(&gdt, 3);

    // Setup registers.
    // cpu::set_cs(1 /*, false, 0 */);
    cpu::set_ds(2, false, 0);
    cpu::set_es(2, false, 0);
    cpu::set_fs(2, false, 0);
    cpu::set_gs(2, false, 0);
    cpu::set_ss(2, false, 0);
}
