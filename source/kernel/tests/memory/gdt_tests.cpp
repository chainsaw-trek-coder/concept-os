#include <gtest/gtest.h>

#if defined(__i386__)
#include "memory/x86_32/gdt.hpp"
#endif

TEST(MemoryTests, segment_descriptor_can_set_type)
{

  segment_descriptor descriptor;

  descriptor.set_type(segment_type::execute_read);

  auto type_mask = 0xF00;
  auto dword1 = descriptor.dword1 & type_mask;

  EXPECT_EQ(dword1, 0xA00);
}

TEST(MemoryTests, segment_descriptor_can_set_base_address)
{

  segment_descriptor descriptor;

  unsigned address = 0xBAADF00D;

  descriptor.set_base_address(reinterpret_cast<void *>(address));

  auto address_part_1_mask = 0xFF000000;
  auto address_part_2_mask = 0x000000FF;
  auto address_part_3_mask = 0xFFFF0000;

  EXPECT_EQ(descriptor.dword1 & address_part_1_mask, address & address_part_1_mask);
  EXPECT_EQ(descriptor.dword1 & address_part_2_mask, (address >> 16) & address_part_2_mask);
  EXPECT_EQ(descriptor.dword2 & address_part_3_mask, (address << 16) & address_part_3_mask);
}

TEST(MemoryTests, segment_descriptor_can_set_limit)
{

  segment_descriptor descriptor;

  unsigned address = 0xBAADF00D;
  size_t limit = 1024;

  descriptor.set_base_address(reinterpret_cast<void *>(address));
  descriptor.set_limit(limit);

  auto limit_mask = 0xFFFF;

  EXPECT_EQ(descriptor.dword2 & limit_mask, limit & limit_mask);
}

TEST(MemoryTests, segment_descriptor_can_set_granularity_flag)
{

  segment_descriptor descriptor;

  unsigned address = 0xBAADF00D;
  size_t limit = 1024;

  descriptor.set_base_address(reinterpret_cast<void *>(address));
  descriptor.set_limit(limit);
  descriptor.set_granularity_flag();

  auto flag_mask = 0x00800000;

  EXPECT_GE(descriptor.dword1 & flag_mask, 0);

  descriptor.clear_granularity_flag();

  EXPECT_EQ(descriptor.dword1 & flag_mask, 0);

  descriptor.set_granularity_flag();

  EXPECT_GE(descriptor.dword1 & flag_mask, 0);
}

TEST(MemoryTests, segment_descriptor_can_set_present)
{

  segment_descriptor descriptor;

  unsigned address = 0xBAADF00D;
  size_t limit = 1024;
  auto flag_mask = 0x00008000;

  descriptor.set_base_address(reinterpret_cast<void *>(address));
  descriptor.set_limit(limit);

  descriptor.set_present(false);
  EXPECT_EQ(descriptor.dword1 & flag_mask, 0);

  descriptor.set_present(true);
  EXPECT_GE(descriptor.dword1 & flag_mask, 0);

  descriptor.set_present(false);
  EXPECT_EQ(descriptor.dword1 & flag_mask, 0);
}