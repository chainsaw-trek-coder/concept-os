#include <gtest/gtest.h>

#if defined(__i386__)
#include "memory/x86_32/gdt.hpp"
#endif

TEST(MemoryTests, segment_descriptor_set_type) {

  segment_descriptor descriptor;

  descriptor.set_type(segment_type::execute_read);

  auto type_mask = 0xF00;
  auto dword1 = descriptor.dword1 & type_mask;

  // Expect equality.
  EXPECT_EQ(dword1, 0xA00);
}