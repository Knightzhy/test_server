#include <stdio.h>
#include <unistd.h>
#include "rpc.h"

#include <gtest/gtest.h>
TEST(RPC, RPCH)
{
    rpc::Header header;
    header.magic = 0x55443322;
    header.length = 1234;
    void *buffer = reinterpret_cast<void *>(&header);
    ssize_t length = rpc::Rpc::GetMessageLength(buffer, sizeof(rpc::Header));
    EXPECT_GT(length, 0);
    printf("length=%u\n", static_cast<uint32_t>(length));
}

TEST(RPC, RPCM)
{
    rpc::Message message;
    message.magic = 0x55443322;
    message.length = 12345;
    const char msg[] = "ABC";
    message.msg = msg[0];
    void *buffer = reinterpret_cast<void *>(&message);
    ssize_t length = rpc::Rpc::GetMessageLength(buffer, sizeof(rpc::Message));
    EXPECT_GT(length, 0);
    printf("length=%u, msg=%s\n", static_cast<uint32_t>(length), message.msg);
}
int main(int argc, char*argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
