#include <gtest/gtest.h>

#include "mocks.hpp"

#include <iuring/SendPacket.hpp>

using testing::_;


namespace Tests
{
    TEST(TestSendPacket, test_sp)
    {
        iuring::SendPacket sp;
        sp.append("hello");
        ASSERT_EQ(sp.size(), strlen("hello"));
        sp.append("");
        ASSERT_EQ(sp.size(), strlen("hello"));

        sp.append_byte('a');
        ASSERT_EQ(sp.size(), strlen("hello") + 1);
        ASSERT_EQ(sp.to_string(), "helloa");

        sp.reset();
        ASSERT_EQ(sp.size(), 0);
        sp.append_byte('a');

        // should be stored in network byteorder:
        sp.append_uint16('b' << 8 | 'c');

        // should be stored in network byteorder:
        sp.append_uint32('d' << 24 | 'e' << 16 | 'f' << 8 | 'g');

        ASSERT_EQ(sp.to_string(), "abcdefg");
    }
}