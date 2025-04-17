#include <gtest/gtest.h>

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

extern "C" void * alloc_for_rust(u64 size)
{
    auto result = malloc(size);

    // Output result for logging
    printf("Allocated %llu bytes at %p\n", size, result);
    return result;
}

extern "C" void dealloc_for_rust(void *ptr)
{
    free(ptr);
}

// Alias rust pointer
typedef u64 partition_rust_callback;

struct PartitionUnsafe
{
    void (*get_size_async)(u64 handle, partition_rust_callback callback);
    void (*read_async)(u64 handle, u8 *buffer, u64 offset, u64 size, partition_rust_callback callback);
    void (*write_async)(u64 handle, const u8 *buffer, u64 offset, u64 size, partition_rust_callback callback);
    void (*flush_async)(u64 handle, partition_rust_callback callback);
};

extern "C" void partition_complete_future(u64 result, partition_rust_callback callback);

class PartitionBase
{
private:
    PartitionUnsafe partition;

public:
    PartitionBase()
    {
        partition = {
            .get_size_async = [](u64 handle, partition_rust_callback callback) {
                PartitionBase *partition = reinterpret_cast<PartitionBase *>(handle);
                u64 size = partition->get_size();
                partition_complete_future(size, callback);
            },
            .read_async = [](u64 handle, u8 *buffer, u64 offset, u64 size, partition_rust_callback callback) {
                PartitionBase *partition = reinterpret_cast<PartitionBase *>(handle);
                u64 bytes_read = partition->read(buffer, offset, size);
                partition_complete_future(bytes_read, callback);
            },
            .write_async = [](u64 handle, const u8 *buffer, u64 offset, u64 size, partition_rust_callback callback) {
                PartitionBase *partition = reinterpret_cast<PartitionBase *>(handle);
                u64 bytes_written = partition->write(buffer, offset, size);
                partition_complete_future(bytes_written, callback);
            },
            .flush_async = [](u64 handle, partition_rust_callback callback) {
                PartitionBase *partition = reinterpret_cast<PartitionBase *>(handle);
                u64 bytes_flushed = partition->flush();
                partition_complete_future(bytes_flushed, callback);
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