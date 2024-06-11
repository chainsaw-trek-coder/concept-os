#include "x86_32/globals.hpp"
#include "x86_32/cpu.h"
#include "memory/utils.h"
#include "x86_32/globals.hpp"

// Intel recommends alignment on 8-byte boundary for best performance.
flat_global_descriptor_table __attribute__((aligned(8))) gdt;
void *global_mem_start = nullptr;
unsigned global_mem_size = 0;

// interrupt_descriptor_table __attribute__((aligned(8))) idt;

unsigned end_of_kernel_loader_variable __attribute__((section(".end_of_kernel_loader")));
void *end_of_kernel_loader = &end_of_kernel_loader_variable;

memory_blocks memory;

page_directory *kernel_page_directory = nullptr;

void initialize_paging()
{
    // Map all memory into pages.
    kernel_page_directory = new (memory.allocate(sizeof(page_directory))) page_directory();

    auto bytes_needed = reinterpret_cast<unsigned>(reinterpret_cast<unsigned char*>(global_mem_start) + global_mem_size);
    // auto bytes_needed = reinterpret_cast<unsigned>(get_aligned_address(end_of_kernel_loader));
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
        entry.set_address(new (memory.allocate(sizeof(page_table))) page_table());
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
    auto memory_start = get_aligned_address(end_of_kernel_loader);
    auto available_memory_size = global_mem_size - (reinterpret_cast<unsigned>(end_of_kernel_loader) - reinterpret_cast<unsigned>(global_mem_start));

    memory.initialize(memory_start, available_memory_size);

    initialize_paging();

    // Enable paging
    cpu::set_page_directory(kernel_page_directory);
    cpu::enable_paging();
}