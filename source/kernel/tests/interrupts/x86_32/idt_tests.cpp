#include <gtest/gtest.h>

#if defined(__i386__)
#include "interrupts/x86_32/idt.hpp"

void dummy_interrupt_handler()
{
    // Do nothing...
}

TEST(InterruptDescriptorTests, interrupt_gate_can_set_code_segment)
{
    auto code_segment = 0xF00D;
    interrupt_gate gate;

    gate.set_code_segment(code_segment);

    auto segment_mask = 0xFFFF0000;
    EXPECT_EQ(gate.dword2 & segment_mask, code_segment << 16);
}

TEST(InterruptDescriptorTests, interrupt_gate_can_set_entry_point)
{
    interrupt_gate gate;

    gate.set_entry_point(dummy_interrupt_handler);

    auto address = reinterpret_cast<unsigned>(dummy_interrupt_handler);

    auto segment_mask_1 = 0xFFFF0000;
    EXPECT_EQ(gate.dword1 & segment_mask_1, address & segment_mask_1);

    auto segment_mask_2 = 0x0000FFFF;
    EXPECT_EQ(gate.dword2 & segment_mask_2, address & segment_mask_2);
}

TEST(InterruptDescriptorTests, interrupt_gate_can_set_present)
{
    interrupt_gate gate;

    auto present_mask = 0x1 << 15;

    gate.set_entry_point(dummy_interrupt_handler);

    gate.set_present(true);
    // EXPECT_EQ(gate.is_present(), true);
    EXPECT_EQ((gate.dword1 & present_mask) > 0, true);

    gate.set_present(false);
    // EXPECT_EQ(gate.is_present(), false);
    EXPECT_EQ(gate.dword1 & present_mask, 0);

    gate.set_present(true);
    // EXPECT_EQ(gate.is_present(), true);
    EXPECT_EQ((gate.dword1 & present_mask) > 0, true);
}

TEST(InterruptDescriptorTests, interrupt_gate_can_set_priviledge)
{
    interrupt_gate gate;

    auto field_mask = 0x3 << 13;

    gate.set_entry_point(dummy_interrupt_handler);

    for (int i = interrupt_priviledge_level::ring_3; i >= interrupt_priviledge_level::ring_0; i--)
    {
        gate.set_priviledge_level(static_cast<interrupt_priviledge_level>(i));
        EXPECT_EQ(gate.dword1 & field_mask, i << 13);
    }
}

TEST(InterruptDescriptorTests, interrupt_gate_can_set_32_bit)
{
    interrupt_gate gate;

    auto field_mask = 0x1 << 11;

    gate.set_entry_point(dummy_interrupt_handler);

    gate.set_32_bit_gate(true);
    EXPECT_EQ((gate.dword1 & field_mask) > 0, true);

    gate.set_32_bit_gate(false);
    EXPECT_EQ(gate.dword1 & field_mask, 0);

    gate.set_32_bit_gate(true);
    EXPECT_EQ((gate.dword1 & field_mask) > 0, true);
}

#endif