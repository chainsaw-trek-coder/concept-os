#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "string_utils.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#if defined(__i386__)
#include "x86_32/multiboot.h"
#endif

#if defined(__x86_64__)
#include "x86_64/multiboot.h"
#endif

#include "terminal.h"

// Doesn't work in protected mode...
// void get_physical_memory(short *low_mem, short *high_mem)
// {
// 	short a = 0, b = 0;

// 	asm volatile(
// 		"xor %%ecx, %%ecx\n\t"
// 		"xor %%edx, %%edx\n\t"
// 		"movw $0xE801, %%eax\n\t"
// 		"int $0x15\n\t"
// 		"movw %%ax, %[low]\n\t"
// 		"movw %%bx, %[high]\n\t"
// 		: [low] "=r"(a), [high] "=r"(b));

// 	*low_mem = a;
// 	*high_mem = b;
// }

extern "C" void kernel_main(multiboot_info_t *mbd, uint32_t magic)
{
	/* Initialize terminal interface */
	terminal_initialize();

	if (magic == MULTIBOOT_BOOTLOADER_MAGIC)
	{
		terminal_writestring("Multiboot information valid.\n\n");

		auto mem_table = (multiboot_memory_map_t*)(mbd->mmap_addr);

		char string_buffer[11];

		for (int i = 0; i < mbd->mmap_length && i < 10; i++)
		{
			auto &mem_entry = mem_table[i];

			if (mem_entry.type > 0)
			{
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
			}
		}
	}
	else
	{
		terminal_writestring("Multiboot information is not valid.\n");
	}
}