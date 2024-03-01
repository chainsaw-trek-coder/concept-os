void* global_mem_start = nullptr;

#if defined(__i386__)
    unsigned global_mem_size = 0;
#endif

#if defined(__x86_64__)
    unsigned long long global_mem_size = 0;
#endif