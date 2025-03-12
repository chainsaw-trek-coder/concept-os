#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "string_utils.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__) && !defined(__testing__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__) && !defined(__testing__)
#error "i386 is the only thing supported."
#endif

#if defined(__i386__)
#include "x86_32/globals.hpp"
#include "x86_32/multiboot.h"
#include "x86_32/cpu.h"
#endif

#include "terminal.h"

void print_mem_entry(multiboot_memory_map_t &mem_entry)
{
	char string_buffer[11];

#if defined(__i386__)
	terminal_writestring("Start Addr: ");
	int_to_hex_string(mem_entry.addr_high, string_buffer);
	terminal_writestring(string_buffer);
	terminal_writestring(" ");
	int_to_hex_string(mem_entry.addr_low, string_buffer);
	terminal_writestring(string_buffer);

	terminal_writestring(" | Length: ");
	int_to_hex_string(mem_entry.len_high, string_buffer);
	terminal_writestring(string_buffer);
	terminal_writestring(" ");
	int_to_hex_string(mem_entry.len_low, string_buffer);
	terminal_writestring(string_buffer);

	terminal_writestring(" | Size: ");
	int_to_hex_string(mem_entry.size, string_buffer);
	terminal_writestring(string_buffer);

	terminal_writestring(" | Type: ");
	int_to_string(mem_entry.type, string_buffer);
	terminal_writestring(string_buffer);
	terminal_writestring("\n");
#else
#if !defined(__testing__)
#error "Printing memory entries for architectures other than i386 not yet supported."
#else
#warning "Please generate 64-bit terminal logic."
#endif
#endif
}

void set_global_memory(multiboot_memory_map_t &mem_entry)
{
#if defined(__i386__)
	if (global_mem_size < mem_entry.len_low)
	{
		global_mem_start = reinterpret_cast<void *>(mem_entry.addr_low);
		global_mem_size = mem_entry.len_low;
	}
#else
#if !defined(__testing__)
#error "Setting global memory for architectures other than i386 not yet supported."
#endif
#endif
}

extern "C" void kernel_main(multiboot_info_t *mbd, 
	uint32_t magic,
	mem_allocator mem_blocks)
{
	/* Initialize terminal interface */
	terminal_initialize();

	if (magic == MULTIBOOT_BOOTLOADER_MAGIC)
	{
		terminal_writestring("Multiboot information valid.\n\n");

		// Select main memory segment.
		auto mem_table = (multiboot_memory_map_t *)(mbd->mmap_addr);

		for (multiboot_uint32_t i = 0; i < mbd->mmap_length && i < 10; i++)
		{
			auto &mem_entry = mem_table[i];

			if (mem_entry.type > 0)
			{
				// print_mem_entry(mem_entry);
			}

			if (mem_entry.type == MULTIBOOT_MEMORY_AVAILABLE)
			{
				set_global_memory(mem_entry);
			}
		}

		char string_buffer[11];
		terminal_writestring("\n\n");
		terminal_writestring("Selected global address: ");
		ptr_to_hex_string(global_mem_start, string_buffer);
		terminal_writestring(string_buffer);
		terminal_writestring("\n");

		terminal_writestring("Selected global size: ");
		int_to_string(global_mem_size, string_buffer);
		terminal_writestring(string_buffer);
		terminal_writestring(" bytes\n");

		terminal_writestring("Setting up segmentation...\n");
		initialize_segmentation();
		terminal_writestring("Finished setting up segmentation...\n");

		memory = mem_blocks;

		terminal_writestring("Memory available: ");
		int_to_string(memory.free_space, string_buffer);
		terminal_writestring(string_buffer);
		terminal_writestring(" bytes\n");

		// Set interrupt table.
		cpu::set_idtr(&idt, 256);

		terminal_writestring("Location of idt is ");
		ptr_to_hex_string(&idt, string_buffer);
		terminal_writestring(string_buffer);
		terminal_writestring("\n");

		terminal_writestring("Location of GDT is ");
		ptr_to_hex_string(&gdt, string_buffer);
		terminal_writestring(string_buffer);
		terminal_writestring("\n");

		// TODO: Save kernel page directory into variable.
		terminal_writestring("Kernel page directory is ");
		ptr_to_hex_string(reinterpret_cast<void *>(kernel_page_directory), string_buffer);
		terminal_writestring(string_buffer);
		terminal_writestring("\n");

		terminal_writestring("Data segment dword1: ");
		ptr_to_hex_string(reinterpret_cast<void *>(gdt.data_segment.dword1), string_buffer);
		terminal_writestring(string_buffer);
		terminal_writestring("\n");

		terminal_writestring("Data segment dword2: ");
		ptr_to_hex_string(reinterpret_cast<void *>(gdt.data_segment.dword2), string_buffer);
		terminal_writestring(string_buffer);
		terminal_writestring("\n");

	}
	else
	{
		terminal_writestring("Multiboot information is not valid.\n");
	}
}
