#if defined(__i386__)
typedef unsigned address_type;
#endif

void* get_aligned_address(void* address)
{
    auto correction = 0x1000 - (reinterpret_cast<address_type>(address) & 0xFFF);
    return reinterpret_cast<void*>((reinterpret_cast<address_type>(address) + correction));
}