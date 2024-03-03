#include <gtest/gtest.h>

#if defined(__i386__)
#include "memory/x86_32/paging.hpp"
#endif

// TEST(MemoryTests, page_directory_entry_can_address)
// {
//     page_directory_entry entry;
    
//     // Allocate more memory than we need so that we can align on 4K boundary.
//     void* data = new char[2 * 4096];

//     void* aligned_data = (reinterpret_cast<unsigned>(data) % 0x3F)
// }