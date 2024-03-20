#include <stddef.h>

extern void* kalloc(size_t size);
extern bool kfree(void* block);