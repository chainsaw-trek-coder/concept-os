#include <gtest/gtest.h>

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

struct PartitionUnsafe
{
    u64 (*get_size)(u64 handle);
    u64 (*read)(u64 handle, u8 *buffer, u64 offset, u64 size);
    u64 (*write)(u64 handle, const u8 *buffer, u64 offset, u64 size);
    u64 (*flush)(u64 handle);
};

class PartitionBase
{
private:
    PartitionUnsafe partition;

public:
    PartitionBase()
    {
        partition = {
            .get_size = [](u64 handle) -> u64
            {
                return reinterpret_cast<PartitionBase *>(handle)->get_size();
            },
            
            .read = [](u64 handle, u8 *buffer, u64 offset, u64 size) -> u64
            {
                return reinterpret_cast<PartitionBase *>(handle)->read(buffer, offset, size);
            },

            .write = [](u64 handle, const u8 *buffer, u64 offset, u64 size) -> u64
            {
                return reinterpret_cast<PartitionBase *>(handle)->write(buffer, offset, size);
            },

            .flush = [](u64 handle) -> u64
            {
                return reinterpret_cast<PartitionBase *>(handle)->flush();
            }
        };
    }

    PartitionUnsafe *get_partition_for_rust() { return &partition; }
    virtual u64 get_size() = 0;
    virtual u64 read(u8 *buffer, u64 offset, u64 size) = 0;
    virtual u64 write(const u8 *buffer, u64 offset, u64 size) = 0;
    virtual u64 flush() = 0;
};

class MemoryPartition : public PartitionBase
{
public:
    u8 buffer[512 * 32] = {0};

    u64 get_size() override { return sizeof(buffer); }

    u64 read(u8 *buffer, u64 offset, u64 size) override
    {
        memcpy(buffer, this->buffer + offset, size);
        return size;
    }

    u64 write(const u8 *buffer, u64 offset, u64 size) override
    {
        memcpy(this->buffer + offset, buffer, size);
        return size;
    }

    u64 flush() override { return 0; }

};

extern "C" int
format_partition(PartitionUnsafe *partition, u64 handle);

TEST(FS_LIB_TESTS, can_format_partition)
{
    MemoryPartition partition;
    PartitionUnsafe *partition_unsafe = partition.get_partition_for_rust();
    int result = format_partition(partition_unsafe, reinterpret_cast<u64>(&partition));

    EXPECT_EQ(*reinterpret_cast<u64*>(&partition.buffer[0]), 0); // Files
    EXPECT_EQ(*reinterpret_cast<u64*>(&partition.buffer[8]), 512); // Free blocks
    EXPECT_EQ(*reinterpret_cast<u64*>(&partition.buffer[16]), 16*512); // Journal address
    EXPECT_EQ(*reinterpret_cast<u64*>(&partition.buffer[24]), 16); // Journal size
    EXPECT_EQ(*reinterpret_cast<u64*>(&partition.buffer[32]), 0); // Current transaction size

    EXPECT_EQ(result, 0);
}