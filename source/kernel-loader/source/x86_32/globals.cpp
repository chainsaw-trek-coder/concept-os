#include "x86_32/globals.hpp"
#include "x86_32/cpu.h"
#include "memory/utils.h"
#include "memory_stream.hpp"
#include "elf_reader.hpp"

// Intel recommends alignment on 8-byte boundary for best performance.
flat_global_descriptor_table __attribute__((aligned(8))) gdt;
void *global_mem_start = nullptr;
unsigned global_mem_size = 0;

// interrupt_descriptor_table __attribute__((aligned(8))) idt;

unsigned end_of_kernel_loader_variable __attribute__((section(".end_of_kernel_loader")));
void *end_of_kernel_loader = &end_of_kernel_loader_variable;

mem_allocator memory;

page_directory *kernel_page_directory = nullptr;

void initialize_paging()
{
    // Map all memory into pages.
    kernel_page_directory = new (memory.allocate(sizeof(page_directory))) page_directory();

    auto bytes_needed = reinterpret_cast<unsigned>(reinterpret_cast<unsigned char *>(global_mem_start) + global_mem_size);
    // auto bytes_needed = reinterpret_cast<unsigned>(get_aligned_address(end_of_kernel_loader));
    auto pages_needed = number_of_blocks(bytes_needed, 4096);
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

    memory.initialize(memory_start, available_memory_size, 4096, false);

    initialize_paging();

    // Enable paging
    cpu::set_page_directory(kernel_page_directory);
    cpu::enable_paging();
}

void load_kernel()
{
    // TODO: Change this to load the kernel from the file system.
    //os::bytes kernel_file(kernel_x86_32, kernel_x86_32_size);
    os::bytes kernel_file = os::bytes(nullptr, 0);
    os::memory_stream stream(kernel_file);
    os::elf_reader reader(stream);

    // Ascertain how much memory we need for the kernel.
    unsigned memory_needed = 0;

    for(auto i = 0; i < reader.get_number_of_section_header_entries(); i++)
    {
        reader.set_current_section_index(i);
        auto &section = reader.get_current_section();

        if(section.is_allocated())
        {
            if(section.address_in_memory + section.section_size > memory_needed)
                memory_needed = section.address_in_memory + section.section_size;
        }
    }

    // Allocate memory
    auto kernel_memory = memory.allocate_protected(memory_needed);

    for(auto i = 0; i < reader.get_number_of_section_header_entries(); i++)
    {
        reader.set_current_section_index(i);
        auto &section = reader.get_current_section();

        if(section.is_allocated())
        {
            if(stream.seek(section.offset_in_file))
            {
                os::bytes kernel_memory_segment = kernel_memory.get_subsection(section.address_in_memory, section.section_size);
                stream.read_bytes(kernel_memory_segment, section.section_size);

                // TODO: Mark as writable based on the section settings.
            }
        }
    }

    // Relocate kernel to high memory
    auto kernel_pages_needed = number_of_blocks(memory_needed, 4096);
    auto new_higher_half_base = reinterpret_cast<void*>(0xFFFFFFFF - (kernel_pages_needed * 4096));
    auto pages_copied = 0;
    bool has_mapped_pages = false;

    for(unsigned page_index = 0; page_index < kernel_pages_needed; page_index++)
    {
        void *old_page_address = reinterpret_cast<unsigned char*>(kernel_memory.get_data()) + ((page_index - pages_copied) * 4096);        
        void *new_page_address = reinterpret_cast<unsigned char*>(new_higher_half_base) + (page_index * 4096);
        
        // Determine if page is exists in physical memory.
        if(kernel_page_directory->is_mapped(new_page_address))
        {
            // If so move it.
            if(has_mapped_pages)
            {
                // TODO: Throw exception/kerenl panic.
                // This section indicates that we mapped some kernel pages to high memory already and now copying additional pages which points to a layout hard to rationalize.
            }

            // Allocate page at this address
            os::bytes allocated_page = memory.allocate_at_protected(new_page_address, 4096);
            
            // Copy data at this new address.
            allocated_page.write_data(0, kernel_memory, 0, 4096);

            // Remove location of kernel.
            // Note this code feels odd because of all the pointers being adjusted internally.
            // Because memory is not set to track sizes it should all work out.
            auto current_kernel_address = kernel_memory.get_data();
            kernel_memory = kernel_memory.get_subsection(4096, kernel_memory.get_size() - 4096);
            memory.deallocate(current_kernel_address); // Because size tracking should be turned off this will just deallocate a block (4096 bytes).
            pages_copied++;
        }
        else
        {
            // If not map kernel page to this location.
            kernel_page_directory->map(memory, new_page_address, old_page_address);
            has_mapped_pages = true;
        }
        
        // Make sure to set the proper page flags.
        auto old_pte = kernel_page_directory->get_page_entry(old_page_address);
        auto new_pte = kernel_page_directory->get_page_entry(new_page_address);

        new_pte->set_writable(old_pte->is_writable());
        new_pte->set_global_page(true);
    }
}
