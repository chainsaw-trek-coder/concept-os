// Contains structures for the Global Descriptor Table.

#include <stddef.h>

enum segment_type
{
    // Data segments
    read_only = 0,
    read_only_accessed = 1,
    read_write = 2,
    read_write_accessed = 3,
    read_only_expand_down = 4,
    read_only_expand_down_accessed = 5,
    read_write_expand_down = 6,
    read_write_expand_down_accessed = 7,

    // Code segments
    execute_only = 8,
    execute_only_accessed = 9,
    execute_read = 10,
    execute_read_accessed = 11,
    execute_only_conforming = 12,
    execute_only_conforming_accessed = 13,
    execute_read_only_conforming = 14,
    execute_read_only_conforming_accessed = 15,

    // System segments
    tss_available_16_bit = 1,
    ldt = 2,
    tss_busy_16_bit = 3,
    call_gate_16_bit = 4,
    task_gate = 5,
    interrupt_gate_16_bit = 6,
    trap_gate_16_bit = 7,
    tss_available_32_bit = 9,
    tss_busy_32_bit = 11,
    call_gate_32_bit = 12,
    interrupt_gate_32_bit = 14,
    trap_gate_32_bit = 15
};

/*

    Segemnt descriptor used in descriptor tables.

    What we are NOT supporting includes:
     - 16-bit mode
     - IA-32e

    As a result some flags will be set/cleared and not allowed to change.

    We're going to default present to true as well.

*/
struct segment_descriptor
{
    unsigned dword1;
    unsigned dword2;

    segment_descriptor();

    void set_type(segment_type type);

    void set_base_address(void* address);

    void set_limit(size_t limit);

    void set_granularity_flag(); // When set, segment size can range from 4 KB to 4 Gigs.
    void clear_granularity_flag(); // When cleared, segment size can range from 1 byte to 1 MB.

    void set_present(bool is_present); // Sets whether or not the segment is currently in memory.

    void set_priviledge_level(unsigned char priviledge);

    void set_is_system(bool is_system); // When set indicates system segment, otherwise code or data.
};

template <int array_size>
struct global_descriptor_table
{
    segment_descriptor null_segment; // Null descriptor is never used.
    segment_descriptor segments[array_size];
};

struct flat_global_descriptor_table
{
    segment_descriptor null_segment; // Null descriptor is never used.
    segment_descriptor code_segment;
    segment_descriptor data_segment;

    flat_global_descriptor_table(void* base, size_t size_in_bytes);
};