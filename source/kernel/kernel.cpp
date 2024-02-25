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
	// HACK
	#undef __x86_64__
#endif

#if defined(__x86_64__)
	#include "x86_64/multiboot.h"
#endif

/* Hardware text mode color constants. */
enum vga_color
{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t)uc | (uint16_t)color << 8;
}

size_t strlen(const char *str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t *terminal_buffer;

void terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t *)0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c)
{
	if (c != '\n')
	{
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

		if (++terminal_column == VGA_WIDTH)
		{
			terminal_column = 0;
			if (++terminal_row == VGA_HEIGHT)
				terminal_row = 0;
		}
	}
	else
	{
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;

		terminal_column = 0;
	}
}

void terminal_write(const char *data, size_t size)
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char *data)
{
	terminal_write(data, strlen(data));
}

void get_physical_memory(short *low_mem, short *high_mem)
{
	short a = 0, b = 0;

	asm volatile(
		"xor %%ecx, %%ecx\n\t"
		"xor %%edx, %%edx\n\t"
		"movw $0xE801, %%eax\n\t"
		"int $0x15\n\t"
		"movw %%ax, %[low]\n\t"
		"movw %%bx, %[high]\n\t"
		: [low] "=r" (a), [high] "=r" (b)
	);

	*low_mem = a;
	*high_mem = b;
}


extern "C" void kernel_main(multiboot_info_t* mbd, uint32_t magic)
{
	/* Initialize terminal interface */
	terminal_initialize();

	if(magic == MULTIBOOT_BOOTLOADER_MAGIC)
		terminal_writestring("Multiboot information valid.");
	else
		terminal_writestring("Multiboot information is not valid.");

	auto mem_table = (multiboot_memory_map_t*)(mbd->mmap_addr);

	char string_buffer[11];

	for(int i = 0; i < mbd->mmap_length && i < 5; i++)
	{

		auto& mem_entry = mem_table[i];

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