#include <gtest/gtest.h>

#if defined(__i386__)
#include "memory/x86_32/gdt.hpp"
#endif

TEST(MemoryTests, segment_descriptor_can_set_type) {

  segment_descriptor descriptor;

  descriptor.set_type(segment_type::execute_read);

  auto type_mask = 0xF00;
  auto dword1 = descriptor.dword1 & type_mask;

  EXPECT_EQ(dword1, 0xA00);
}

TEST(MemoryTests, segment_descriptor_can_set_base_address) {

  segment_descriptor descriptor;

  unsigned address = 0xBAADF00D;

  descriptor.set_base_address(reinterpret_cast<void*>(address));

  auto address_part_1_mask = 0xFF000000;
  auto address_part_2_mask = 0x000000FF;
  auto address_part_3_mask = 0xFFFF0000;

  EXPECT_EQ(descriptor.dword1 & address_part_1_mask, address & address_part_1_mask);
  EXPECT_EQ(descriptor.dword1 & address_part_2_mask, (address >> 16) & address_part_2_mask);
  EXPECT_EQ(descriptor.dword2 & address_part_3_mask, (address << 16) & address_part_3_mask);
}