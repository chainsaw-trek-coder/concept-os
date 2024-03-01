extern void* global_mem_start;

#if defined(__i386__)
    extern unsigned global_mem_size;
#endif

#if defined(__x86_64__)
    extern unsigned long long global_mem_size;
#endif