#include "x86_32/globals.hpp"
#include "x86_32/cpu.h"
#include "memory/utils.h"
#include "memory/memory_blocks.hpp"
#include "x86_32/globals.hpp"

// Intel recommends alignment on 8-byte boundary for best performance.
flat_global_descriptor_table __attribute__((aligned(8))) gdt;
void *global_mem_start = nullptr;
unsigned global_mem_size = 0;

interrupt_descriptor_table __attribute__((aligned(8))) idt;

unsigned end_of_kernel_variable __attribute__((section(".end_of_kernel")));
void *end_of_kernel = &end_of_kernel_variable;

memory_blocks mem_blocks;

page_directory *kernel_page_directory = nullptr;

void initialize_segmentation()
{
    // Setup gdt.
    // auto &segment = gdt.segments[0];
    // segment.set_base_address(nullptr);
    // segment.clear_granularity_flag(); // Byte sizes
    // segment.set_is_system(true);
    // segment.set_limit(0xFFFFFFFF);
    // segment.set_present(true);
    // segment.set_priviledge_level(0);
    // segment.set_type(segment_type::read_write_expand_down);

    gdt.initialize();

    cpu::set_gdtr(&gdt,3);

    // Setup registers.
    // cpu::set_cs(1 /*, false, 0 */);
    cpu::set_ds(2, false, 0);
    cpu::set_es(2, false, 0);
    cpu::set_fs(2, false, 0);
    cpu::set_gs(2, false, 0);
    cpu::set_ss(2, false, 0);
}

void initialize_paging()
{
    // Map kernel into pages.
    kernel_page_directory = new (mem_blocks.allocate(sizeof(page_directory))) page_directory();

    auto bytes_needed = reinterpret_cast<unsigned>(get_aligned_address(end_of_kernel));
    auto pages_needed = number_of_blocks(bytes_needed);
    auto page_tables_needed = pages_needed / 1024 + (pages_needed % 1024 > 0 ? 1 : 0);

    // TODO: Kernel panic if tables needed is more than 1024.
    //       Shouldn't be possible in 32-bit mode because that would indicate the need
    //       to address more than 4 Gigs of memory.

    // Allocate page tables.
    unsigned current_page = 0;

    for (size_t i = 0; i < page_tables_needed; i++)
    {
        auto &entry = kernel_page_directory->entries[i];
        entry.set_address(new (mem_blocks.allocate(sizeof(page_table))) page_table());
        entry.set_present(true);

        auto &table = *entry.get_address();

        // Allocate pages
        for (size_t j = 0; j < 1024 && current_page < pages_needed; j++)
        {
            table.entries[j].set_address(reinterpret_cast<void *>(current_page * 4096));
            table.entries[j].set_global_page(true);
            table.entries[j].set_present(true);
            table.entries[j].set_type(page_entry_type::supervisor);

            current_page++;
        }
    }
}

void initialize_memory()
{
    initialize_segmentation();

    auto memory_start = get_aligned_address(end_of_kernel);
    auto memory_end = reinterpret_cast<unsigned char*>(global_mem_start) + global_mem_size;
    auto available_memory_size = memory_end - reinterpret_cast<unsigned char*>(memory_start);

    mem_blocks.initialize(memory_start, available_memory_size);

    initialize_paging();

    // Enable paging
    cpu::set_page_directory(kernel_page_directory);
    cpu::enable_paging();
}