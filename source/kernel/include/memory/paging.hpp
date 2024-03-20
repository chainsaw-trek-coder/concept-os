#if defined(__i386__)
#include "memory/x86_32/paging.hpp"
#else
#error Paging not implemented for anything other than x86 (32-bit).
#endif
