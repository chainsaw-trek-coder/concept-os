#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "multiboot.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
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

void convert_to_string(unsigned value, char *str)
{
	char *cur_conv = str + 9;

	do
	{
		// Convert current digit to character and store it.
		*cur_conv = 48 + (value % 10);
		cur_conv--;
	} while ((value /= 10) > 0);

	cur_conv++;

	// Left-align string.
	char *cur_align = str;
	while (cur_conv <= str + 9)
	{
		*cur_align = *cur_conv;
		cur_align++;
		cur_conv++;
	}

	*cur_align = 0;
}

extern "C" void kernel_main(multiboot_info_t* mbd, uint32_t magic)
{
	/* Initialize terminal interface */
	terminal_initialize();

	if(magic == MULTIBOOT_BOOTLOADER_MAGIC)
		terminal_writestring("Multiboot information valid.");
	else
		terminal_writestring("Multiboot information is not valid.");

	// short low_mem = 1, high_mem_pages = 1;
	// get_physical_memory(&low_mem, &high_mem_pages);

	// unsigned high_mem = high_mem_pages * 65536;

	// char low_mem_string[11];
	// char high_mem_string[11];
	// convert_to_string(low_mem, low_mem_string);
	// convert_to_string(high_mem, high_mem_string);

	// terminal_writestring("Low memory: ");
	// terminal_writestring(low_mem_string);
	// terminal_writestring("\n");

	// terminal_writestring("High memory: ");
	// terminal_writestring(high_mem_string);
	// terminal_writestring("\n");
}