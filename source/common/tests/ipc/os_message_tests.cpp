#include <gtest/gtest.h>
#include "ipc/os_message.hpp"

using namespace os::ipc;

TEST(OSMessageTests, os_message_can_initialize)
{
    void *shared_mem = new char[1024];

    for (unsigned int i = 0; i < 1024; i++)
        reinterpret_cast<char *>(shared_mem)[i] = 0xFF;

    os_message message(2, shared_mem, 1024);

    EXPECT_EQ(2, *reinterpret_cast<unsigned *>(shared_mem));

    for (unsigned i = 0; i < 4 + sizeof(unsigned) * 4; i++)
    {
        if (i > 4)
            EXPECT_EQ(0, reinterpret_cast<char *>(shared_mem)[i]);
    }

    for (unsigned i = 4 + sizeof(unsigned) * 4; i < 1024; i++)
    {
        if (i > 4)
            EXPECT_EQ(static_cast<char>(0xFF), reinterpret_cast<char *>(shared_mem)[i]);
    }
}

TEST(OSMessageTests, os_message_can_set_two_parameters)
{
    void *shared_mem = new char[1024];

    for (unsigned int i = 0; i < 1024; i++)
        reinterpret_cast<char *>(shared_mem)[i] = 0xFF;

    os_message message(2, shared_mem, 1024);

    auto param1 = "hello";
    auto param2 = "world";

    message.set_value(0, param1, strlen(param1) + 1); // +1 for the null character.
    message.set_value(1, param2, strlen(param2) + 1); // +1 for the null character.

    auto param1Offset = 4 + sizeof(unsigned) * 4;
    auto param1Size = strlen(param1) + 1;

    auto param2Offset = param1Offset + param1Size;
    auto param2Size = strlen(param2) + 1;

    // Check parameter 1 offset and size in memory.
    EXPECT_EQ(param1Offset, reinterpret_cast<unsigned*>(shared_mem)[1]);
    EXPECT_EQ(param1Size, reinterpret_cast<unsigned*>(shared_mem)[2]);

    // Check parameter 2 offset and size in memory.
    EXPECT_EQ(param2Offset, reinterpret_cast<unsigned*>(shared_mem)[3]);
    EXPECT_EQ(param2Size, reinterpret_cast<unsigned*>(shared_mem)[4]);

    for (unsigned i = 0; i < param1Size; i++)
    {
        EXPECT_EQ(param1[i], reinterpret_cast<char *>(shared_mem)[param1Offset + i]);
    }

    for (unsigned i = 0; i < param2Size; i++)
    {
        EXPECT_EQ(param2[i], reinterpret_cast<char *>(shared_mem)[param2Offset + i]);
    }
}

TEST(OSMessageTests, os_message_can_get_two_parameters)
{
    void *shared_mem = new char[1024];

    for (unsigned int i = 0; i < 1024; i++)
        reinterpret_cast<char *>(shared_mem)[i] = 0xFF;

    os_message message(2, shared_mem, 1024);

    auto param1 = "hello";
    auto param2 = "world";

    message.set_value(0, param1, strlen(param1) + 1); // +1 for the null character.
    message.set_value(1, param2, strlen(param2) + 1); // +1 for the null character.

    EXPECT_EQ(strlen(param1) + 1, message.get_value_size(0));
    EXPECT_EQ(strlen(param2) + 1, message.get_value_size(1));

    auto param1_retrieved = reinterpret_cast<char *>(message.get_value(0));
    auto param2_retrieved = reinterpret_cast<char *>(message.get_value(1));

    EXPECT_EQ(0, strcmp(param1, param1_retrieved));
    EXPECT_EQ(0, strcmp(param2, param2_retrieved));
}