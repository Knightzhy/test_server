#include <stdio.h>
#include <unistd.h>
#include "rpc.h"

#include <gtest/gtest.h>
TEST(RPC, A)
{
    rpc::Header header;
    header.magic = 0x55443322;
    header.length = 1234;
    void *buffer = reinterpret_cast<void *>(&header);
    ssize_t length = rpc::Rpc::GetMessageLength(buffer, sizeof(rpc::Header));
    EXPECT_GT(length, 0);
    printf("length=%u\n", static_cast<uint32_t>(length));
}

TEST(RPC, B)
{
    rpc::Message message;
    message.magic = 0x55443322;
    message.length = 12345;
    strcpy(message.msg, "ABC"); // this is wrong, "ABC" size > message.msg
    void *buffer = reinterpret_cast<void *>(&message);
    ssize_t length = rpc::Rpc::GetMessageLength(buffer, sizeof(rpc::Message));
    EXPECT_GT(length, 0);
    printf("length=%u, msg=%s\n", static_cast<uint32_t>(length), message.msg);
}
TEST(RPC, C)
{
    rpc::Message message;
    message.magic = 0x55443322;
    message.length = 12345;
    strcpy(message.msg, "ABC");
    void * buffer = reinterpret_cast<void *>(&message);
    size_t length = sizeof(message);
    std::string msg = rpc::Rpc::Parse(buffer, length);
    printf("msg=%s\n", msg.c_str());
}

TEST(RPC, D)
{
    rpc::Message message;
    printf("msg.length=%d\n", (int)sizeof(message));
    message.magic = 0x55443322;
    message.length = 12345;
    strcpy(message.msg, "ABC"); // this is wrong, "ABC" size > message.msg
    printf("msg.length=%d\n", (int)sizeof(message));
    rpc::Message message1;
    printf("msg.length=%d, msg=%s\n", (int)sizeof(message), message.msg);
    printf("msg1.length=%d, msg=%s\n", (int)sizeof(message), message1.msg);
    message1.magic = 0x55667788;
    message.length = 12345;
    strcpy(message.msg, "DNSJEEE");// this is wrong, "DNSJEEE" size > message.msg
    printf("msg.length=%d, msg=%s\n", (int)sizeof(message), message.msg);
    printf("msg1.length=%d, msg=%s\n", (int)sizeof(message), message1.msg);
}

TEST(RPC, E)
{
    std::string msg = "ABCDNE";
    rpc::Message *message = (rpc::Message *)malloc(sizeof(rpc::Message) + sizeof(msg));
    message->magic = 0x55443322;
    message->length = 12345;
    strcpy(message->msg, msg.c_str());
    printf("msg.length=%d, magic=%x, length=%d, msg=%s\n",
            (int)sizeof(*message),  message->magic, message->length, message->msg);

    printf("======\n");

    msg = "QWERJJSKDFIOIW";
    rpc::Message *message1 = (rpc::Message *)malloc(sizeof(rpc::Message) + sizeof(msg));
    message1->magic = 0x55667788;
    message1->length = 54321;
    strcpy(message1->msg, msg.c_str());
    printf("msg.length=%d, magic=%x, length=%d, msg=%s\n",
            (int)sizeof(*message),  message->magic, message->length, message->msg);
    printf("msg.length=%d, magic=%x, length=%d, msg=%s\n",
            (int)sizeof(*message1),  message1->magic, message1->length, message1->msg);
    free(message);
    free(message1);
}

TEST(RPC, F)
{
    std::string msg = "ABCDNE";
    size_t length = sizeof(rpc::Message) + sizeof(msg);
    void *buffer = (void *)malloc(length);
    rpc::Message *message = reinterpret_cast<rpc::Message *>(buffer);
    message->magic = 0x55443322;
    message->length = 12345;
    strcpy(message->msg, msg.c_str());
    printf("msg.length=%d, magic=%x, length=%d, msg=%s\n",
            (int)sizeof(*message),  message->magic, message->length, message->msg);

    printf("======\n");

    msg = "QWERJJSKDFIOIW";
    size_t length1 = sizeof(rpc::Message) + sizeof(msg);
    void *buffer1 = (void *)malloc(length1);
    rpc::Message *message1 = reinterpret_cast<rpc::Message *>(buffer1);
    message1->magic = 0x55667788;
    message1->length = 54321;
    strcpy(message1->msg, msg.c_str());
    printf("msg.length=%d, magic=%x, length=%d, msg=%s\n",
            (int)sizeof(*message),  message->magic, message->length, message->msg);
    printf("msg.length=%d, magic=%x, length=%d, msg=%s\n",
            (int)sizeof(*message1),  message1->magic, message1->length, message1->msg);

    msg = rpc::Rpc::Parse(buffer, length);
    printf("msg=%s\n", msg.c_str());
    msg = rpc::Rpc::Parse(buffer1, length1);
    printf("msg1=%s\n", msg.c_str());
    free(buffer);
    free(buffer1);
}

int main(int argc, char*argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
