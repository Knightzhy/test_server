#include <stdlib.h>
#include <gtest/gtest.h>
#include <string.h>
#include <malloc.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include "types.h"
#include "test_server/socket_basic/proto/helloworld.pb.h"
using helloworld::HelloRequest;
using helloworld::HelloReply;

int write_protobuf(int fd)
{
    HelloRequest request;
    request.set_name("Knightwu");
    int size = request.ByteSizeLong();
    void *buffer = malloc(size);
    bzero(buffer, size);
    bool result = request.SerializeToArray(buffer, size);
    if (!result) {
        printf("SerializeToArray Failed.\n");
        free(buffer);
        return -1;
    }
    ssize_t count = write(fd, buffer, size);
    printf("write to fd[%d], return [%d].\n", fd, count);
    free(buffer);
    return count;
}

int read_protobuf(int fd)
{
    char buffer[1024000] = {0};
    ssize_t count = read(fd, buffer, 1024000);
    printf("read to fd[%d], return [%d].\n", fd, count);
    if (count <=0) {
        return count;
    }
    HelloReply reply;
    bool result = reply.ParseFromArray(buffer, count);
    if (!result) {
        printf("ParseFromArray Failed.\n");
        return -1;
    }
    printf("read protobuf, address:%s.\n", reply.address().c_str());
    return count;
}

int write_struct(int fd)
{
    common::User u;
    u.attr1 = 101;
    u.attr2 = 201;
    u.attr3 = false;
    ssize_t count = write(fd, (void *)&u, sizeof(u));
    printf("write to fd[%d], return [%d].\n", fd, count);
    return count;
}

int read_struct(int fd)
{
    char buffer[100];
    ssize_t count = read(fd, buffer, 100);
    printf("read from fd[%d], return [%d].\n", fd, count);
    common::User *u = reinterpret_cast<common::User *>(buffer);
    printf("Read, attr1=%d, attr2=%d, attr3=%d\n", u->attr1, u->attr2, u->attr3);
    return count;
}

int write_char(int fd)
{
    char buffer[100] = "Hello World, Lily.";
    ssize_t count = write(fd, buffer, strlen(buffer));
    printf("write [%s] to fd[%d], return [%d].\n", buffer, fd, count);
    return count;
}

int read_char(int fd)
{
    char buffer[100] = {0};
    ssize_t count = read(fd, buffer, 100);
    printf("read [%s] to fd[%d], return [%d].\n", buffer, fd, count);
    return count;
}

int write_structs(int fd)
{
    int num = 10;
    common::Address *address = (common::Address *)malloc(sizeof(common::Address) * num);
    for (int i=0; i< num; i++) {
        address[i].attr1 = 100+i;
        address[i].attr3 = false;
        bzero(address[i].attr2, 100);
        strcpy(address[i].attr2, "This is hellll");
    }

    char *ptr = (char *)malloc(10000);
    bzero(ptr, 10000);
    char *tmp_ptr = ptr;
    int total_length = 0;
    for (int i=0; i<num; i++) {
        int data_length = sizeof(address[i]);
        memcpy(tmp_ptr, &data_length, sizeof(int));
        tmp_ptr += sizeof(int);
        total_length += sizeof(int);
        memcpy(tmp_ptr, &(address[i]), data_length);
        tmp_ptr += data_length;
        total_length += data_length;
        printf("write index: %d, %d\n", i, data_length);
    }
    ssize_t count = write(fd, (void *)ptr, total_length);
    printf("write to fd[%d], return [%d].\n", fd, count);
    free(address);
    free(ptr);
    return count;
}

int read_structs(int fd)
{
    char buffer[1024000] = {0};
    ssize_t count = read(fd, buffer, 1024000);
    printf("read from fd[%d], return [%d].\n", fd, count);
    if (count <=0) {
        return count;
    }
    int num = 20;
    common::Address *address = (common::Address *)malloc(sizeof(common::Address) * num);
    for (int i=0; i< num; i++) {
        bzero(address[i].attr2, 100);
    }

    int remain_length = count;
    char *tmp_ptr = buffer;
    int index = 0;
    do {
        int data_length = 0;
        memcpy(&data_length, tmp_ptr, sizeof(int));
        tmp_ptr += sizeof(int);
        remain_length -= sizeof(int);
        memcpy(reinterpret_cast<void *>(&(address[index])), tmp_ptr, data_length);
        tmp_ptr += data_length;
        remain_length -= data_length;
        printf("read index:%d, %d, %d, %s, %d\n",index, data_length,
                address[index].attr1, address[index].attr2, address[index].attr3);
    } while (index<num && remain_length>0);
    free(address);
    return count;
}

int read_chars(int fd)
{
    char buffer[1024000] = {0};
    ssize_t count = read(fd, buffer, 1024000);
    printf("read to fd[%d], return [%d].\n", fd, count);
    if (count <=0) {
        return count;
    }
    int row = 10;
    int col = 100;
    char ** data = (char **)malloc(sizeof(char *) * row);
    for (int i=0; i< row; i++) {
        data[i] = (char *)malloc(sizeof(char) * col);
        bzero(data[i], sizeof(char)*col);
    }

    int remain_length = count;
    char *tmp_ptr = buffer;
    int index = 0;
    do {
        int data_length = 0;
        memcpy(&data_length, tmp_ptr, sizeof(int));
        tmp_ptr += sizeof(int);
        remain_length -= sizeof(int);
        memcpy(data[index], tmp_ptr, data_length);
        tmp_ptr += data_length;
        remain_length -= data_length;
        printf("read index:%d, %d, %d, %s\n", index, sizeof(int), data_length, data[index]);
        index++;
    } while(index< row && remain_length >0);

    for (int i=0; i< row; i++) {
        free(data[i]);
    }
    free(data);
    return count;
}

int write_chars(int fd)
{
    int row = 5;
    int col = 100;
    char ** data = (char **)malloc(sizeof(char *) * row);
    for (int i=0; i< row; i++) {
        data[i] = (char *)malloc(sizeof(char) * col);
        bzero(data[i], sizeof(char)*col);
    }
    strcpy(data[0], "This is 1.");
    strcpy(data[1], "This is 22.");
    strcpy(data[2], "This is 333.");
    strcpy(data[3], "This is 4444.");
    strcpy(data[4], "This is 55555.");
    char *ptr = (char *)malloc(1000);
    bzero(ptr, 1000);
    char *tmp_ptr = ptr;
    int total_length = 0;;
    for (int i=0; i< 5; i++) {
        int data_length = strlen(data[i]);
        memcpy(tmp_ptr, &data_length, sizeof(int));
        tmp_ptr += sizeof(int);
        total_length += sizeof(int);
        memcpy(tmp_ptr, data[i], data_length);
        tmp_ptr += data_length;
        total_length += data_length;
        printf("write index: %d, %s\n", i, data[i]);
    }
    ssize_t count = write(fd, ptr, total_length);
    printf("write to fd[%d], return [%d].\n", fd, count);

    for (int i=0; i< row; i++) {
        free(data[i]);
    }
    free(data);
    free(ptr);
    return count;
}

int constant_write_chars(int fd)
{
    int row = 5;
    int col = 100;
    char ** data = (char **)malloc(sizeof(char *) * row);
    for (int i=0; i< row; i++) {
        data[i] = (char *)malloc(sizeof(char) * col);
        bzero(data[i], sizeof(char)*col);
    }
    strcpy(data[0], "This is 1.");
    strcpy(data[1], "This is 22.");
    strcpy(data[2], "This is 333.");
    strcpy(data[3], "This is 4444.");
    strcpy(data[4], "This is 55555.");
    char *ptr = (char *)malloc(1000);
    bzero(ptr, 1000);
    char *tmp_ptr = ptr;
    int total_length = 0;;
    for (int i=0; i< 5; i++) {
        int data_length = strlen(data[i]);
        memcpy(tmp_ptr, &data_length, sizeof(int));
        tmp_ptr += sizeof(int);
        total_length += sizeof(int);
        memcpy(tmp_ptr, data[i], data_length);
        tmp_ptr += data_length;
        total_length += data_length;
        printf("write index: %d, %s\n", i, data[i]);
    }
    ssize_t count = write(fd, ptr, total_length);
    printf("write to fd[%d], return [%d].\n", fd, count);

    for (int i=0; i< row; i++) {
        free(data[i]);
    }
    free(data);
    free(ptr);
    return count;
}

int set_nonblockint(int fd) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0))== -1){
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int one_connect(){
    int socket_fd;
    // create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0 ) {
        printf("When create socket, there is a wrong. errno:%d\n", socket_fd);
        return -1;
    }
    printf("create socket success.[%d]\n", socket_fd);
    //set_nonblockint(socket_fd);

    // connet
    sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(55555);
    inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);
    int ret = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        printf("When connect, there is a wrong. ret:%d, errno:%d, %s\n", ret, errno, strerror(errno));
        return -1;
    }
    printf("connect success.\n");
    return socket_fd;
}

TEST(A, CHAR)
{
    int socket_fd = one_connect();
    ASSERT_GT(socket_fd, 0);
//    sleep(3);
    write_char(socket_fd);
    read_char(socket_fd);
    close(socket_fd);
}

TEST(A, STRUCT)
{
    int socket_fd = one_connect();
    ASSERT_GT(socket_fd, 0);
    write_struct(socket_fd);
    read_struct(socket_fd);
    close(socket_fd);
}

TEST(A, PROTO)
{
    int socket_fd = one_connect();
    ASSERT_GT(socket_fd, 0);
    write_protobuf(socket_fd);
    read_protobuf(socket_fd);
    close(socket_fd);
}

TEST(A, CHARS)
{
    int socket_fd = one_connect();
    ASSERT_GT(socket_fd, 0);
    write_chars(socket_fd);
    read_chars(socket_fd);
    close(socket_fd);
}

TEST(A, STRUCTS)
{
    int socket_fd = one_connect();
    ASSERT_GT(socket_fd, 0);
    write_structs(socket_fd);
    read_structs(socket_fd);
    close(socket_fd);
}

TEST(A, PROTOS)
{
}

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
