#include <gtest/gtest.h>

extern "C" int test();

TEST(FS_LIB_TESTS, basic_test)
{
    auto result = test();
    EXPECT_EQ(result, 123456);
}