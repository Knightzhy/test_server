#include <sys/socket.h>
#include <gtest/gtest.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include "types.h"
#include "test_server/socket_basic/proto/helloworld.pb.h"
using helloworld::HelloRequest;
using helloworld::HelloReply;

int write_protobuf(int fd)
{
    HelloReply reply;
    reply.set_address("10C.xxx");
    int size = reply.ByteSizeLong();
    void *buffer = malloc(size);
    bzero(buffer, size);
    bool result = reply.SerializeToArray(buffer, size);
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
    HelloRequest request;
    bool result = request.ParseFromArray(buffer, count);
    if (!result) {
        printf("ParseFromArray Failed.\n");
        return -1;
    }
    printf("read protobuf, name:%s.\n", request.name().c_str());
    return count;
}

int write_struct(int fd)
{
    common::User u;
    u.attr1 = 100;
    u.attr2 = 200;
    u.attr3 = true;
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
    char buffer[100] = "How are you?";
    ssize_t count = write(fd, buffer, strlen(buffer));
    printf("write [%s] to fd[%d], return [%d].\n", buffer, fd, count);
    if (count < 0) {
        printf("write_message error ret=%d.\n", count);
    }
    return count;
}
int read_char(int fd)
{
    char buffer[100]={0};
    ssize_t count = read(fd, buffer, 100);
    printf("read [%s] to fd[%d], return [%d].\n", buffer, fd, count);
    if (count < 0) {
        printf("read_message error ret=%d.\n", count);
        printf("errno:%d, error:%s\n", errno, strerror(errno));
    }
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
    strcpy(data[0], "This is A.");
    strcpy(data[1], "This is BB.");
    strcpy(data[2], "This is CCC.");
    strcpy(data[3], "This is DDDD.");
    strcpy(data[4], "This is EEEEE.");
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

int write_structs(int fd)
{
    int num = 10;
    common::Address *address = (common::Address *)malloc(sizeof(common::Address) * num);
    for (int i=0; i< num; i++) {
        address[i].attr1 = 500+i;
        address[i].attr3 = true;
        bzero(address[i].attr2, 100);
        strcpy(address[i].attr2, "This is sdjskdj");
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

int set_nonblockint(int fd) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0))== -1){
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


int Listen(){
    int listening_fd;

    // create listening fd
    listening_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_fd <= 0 ) {
        printf ("when create socket fd, there is a wrong. errno:%d", listening_fd);
        return -1;
    }
    printf ("success to create socket fd [%d]\n", listening_fd);

    // set socketopt
    int reuseaddr = 1;
    int ret = setsockopt(listening_fd, SOL_SOCKET, SO_REUSEADDR, (const
                char *)&reuseaddr, sizeof(reuseaddr));
    if (ret < 0) {
        printf("When setsockopt reuseaddr, there is a wrong, errno:%d,%s\n", errno,strerror(errno));
        return -1;
    }
    printf("success to set socket");

    // bind addr
    sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(55555);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ret = bind (listening_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if ( ret < 0 ) {
        printf("When bind socket_fd and address, there is a wrong. errno:%d\n", ret);
        return -1;
    }
    printf ("bind success.\n");

    // listening
    if (listen(listening_fd, 5) < 0) {
        printf("When listen to the socket, there is a wrong.\n");
        return -1;
    }
    printf ("listen success\n");
    return listening_fd;

}

int Accept(int listening_fd){
    int linkage_fd;
    sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    int client_addr_len = sizeof(client_addr);
    linkage_fd = accept(listening_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
    if (linkage_fd < 0) {
        printf ("When accept a linkage, there is a wrong. errno:%d\n", linkage_fd);
        return -1;
    }
    printf ("accept a linkage [%d], ip[%s], port[%d]\n",
            linkage_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
//    set_nonblockint(linkage_fd);
    return linkage_fd;
}

TEST(A, CHAR)
{
    int listening_fd = Listen();
    ASSERT_GT(listening_fd, 0);
    int linkage_fd = Accept(listening_fd);
    ASSERT_GT(linkage_fd, 0);
//    set_nonblockint(linkage_fd);
//    int ret;
    int ret = write_char(linkage_fd);
    ASSERT_GT(ret, 0);
//    sleep(3);
    ret = read_char(linkage_fd);
    ASSERT_GT(ret, 0);
    // close listening fd
    close(linkage_fd);
    close(listening_fd);
}

TEST(A, STRUCT)
{
    int listening_fd = Listen();
    ASSERT_GT(listening_fd, 0);
    int linkage_fd = Accept(listening_fd);
    ASSERT_GT(linkage_fd, 0);
    int ret = write_struct(linkage_fd);
    ASSERT_GT(ret, 0);
    ret = read_struct(linkage_fd);
    ASSERT_GT(ret, 0);
    // close listening fd
    close(linkage_fd);
    close(listening_fd);
}

TEST(A, PROTO)
{
    int listening_fd = Listen();
    ASSERT_GT(listening_fd, 0);
    int linkage_fd = Accept(listening_fd);
    ASSERT_GT(linkage_fd, 0);
    int ret = read_protobuf(linkage_fd);
    ASSERT_GT(ret, 0);
    ret = write_protobuf(linkage_fd);
    ASSERT_GT(ret, 0);
    // close listening fd
    close(linkage_fd);
    close(listening_fd);
}

TEST(A, CHARS)
{
    int listening_fd = Listen();
    ASSERT_GT(listening_fd, 0);
    int linkage_fd = Accept(listening_fd);
    ASSERT_GT(linkage_fd, 0);
    int ret = write_chars(linkage_fd);
    ASSERT_GT(ret, 0);
    ret = read_chars(linkage_fd);
    ASSERT_GT(ret, 0);
    // close listening fd
    close(linkage_fd);
    close(listening_fd);
}

TEST(A, STRUCTS)
{
    int listening_fd = Listen();
    ASSERT_GT(listening_fd, 0);
    int linkage_fd = Accept(listening_fd);
    ASSERT_GT(linkage_fd, 0);
    int ret = write_structs(linkage_fd);
    ASSERT_GT(ret, 0);
    ret = read_structs(linkage_fd);
    ASSERT_GT(ret, 0);
    // close listening fd
    close(linkage_fd);
    close(listening_fd);
}

TEST(A, PROTOS)
{
}

/*
 * 多进程
 * */
TEST(B, CHAR)
{
    int listening_fd = Listen();
    ASSERT_GT(listening_fd, 0);
    int linkage_fd =0;
    int num = 3;
    int pid =0;
    do {
        linkage_fd = Accept(listening_fd);
        ASSERT_GT(linkage_fd, 0);
        pid = fork();
        ASSERT_GE(pid, 0);
        if (pid == 0) { // child
            int ret = write_char(linkage_fd);
            ASSERT_GT(ret, 0);
            ret = read_char(linkage_fd);
            ASSERT_GT(ret, 0);
            close(linkage_fd);
            num=0;
        } else { // father
            num--;
            int status;
            wait(&status);
            close(linkage_fd);
            printf("NUM:%d\n", num);
        }
    } while (num>0);
    if (pid >0) {
        close(listening_fd);
        printf("close listenind_fd\n");
    } else {
        printf("close linkage_id\n");
    }
}

void thread_func(int linkage_fd){
    int ret = write_char(linkage_fd);
    ASSERT_GT(ret, 0);
    ret = read_char(linkage_fd);
    ASSERT_GT(ret, 0);
    close(linkage_fd);
    return;
}

/*
 * 多线程
 * */
TEST(C, CHAR) {
    int listening_fd = Listen();
    ASSERT_GT(listening_fd, 0);
    int num =3;
    do {
        int linkage_fd = Accept(listening_fd);
        ASSERT_GT(linkage_fd, 0);
        std::thread t(thread_func, linkage_fd);
        t.join();
        num--;
    } while(num>0);
    close(listening_fd);
}

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
