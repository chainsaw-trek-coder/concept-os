#include "x86_32/globals.hpp"
#include "x86_32/cpu.h"
#include "memory/utils.h"
#include "x86_32/globals.hpp"

// Intel recommends alignment on 8-byte boundary for best performance.
flat_global_descriptor_table __attribute__((aligned(8))) gdt;
void *global_mem_start = nullptr;
unsigned global_mem_size = 0;

interrupt_descriptor_table __attribute__((aligned(8))) idt;

unsigned end_of_kernel_variable __attribute__((section(".end_of_kernel")));
void *end_of_kernel = &end_of_kernel_variable;

page_directory *global_page_directory = nullptr;

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
    // cpu::set_cs(1, false, 0);
    cpu::set_ds(2, false, 0);
    cpu::set_es(2, false, 0);
    cpu::set_fs(2, false, 0);
    cpu::set_gs(2, false, 0);
    cpu::set_ss(2, false, 0);
}

void initialize_paging()
{
    // Map kernel into pages.
    global_page_directory = new (get_aligned_address(end_of_kernel)) page_directory();

    auto pages_needed = reinterpret_cast<unsigned>(global_page_directory) / 0x1000;
    auto page_tables_needed = pages_needed / 1024 + (pages_needed % 1024 > 0 ? 1 : 0);

    // TODO: Kernel panic if tables needed is more than 1024.
    //       Shouldn't be possible in 32-bit mode because that would indicate the need
    //       to address more than 4 Gigs of memory.

    // Allocate page tables.
    auto page_tables = reinterpret_cast<page_table *>(global_page_directory + 1);

    unsigned current_page = 0;

    for (size_t i = 0; i < page_tables_needed; i++)
    {
        auto &entry = global_page_directory->entries[i];
        entry.set_address(new (&page_tables[i]) page_table());
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
    initialize_paging();

    // Enable paging
    cpu::set_page_directory(global_page_directory);
    cpu::enable_paging();
}