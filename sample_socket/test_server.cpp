#include <sys/socket.h>
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
#include <gtest/gtest.h>

#pragma pack(push)
#pragma pack(1)
struct FuncData {
    int fd;
    int (*write_func)(int);
    int (*read_func)(int);
};
#pragma pack(pop)

int set_nonblockint(int fd) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0))== -1){
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void *pthread_func(void* m)
{
    pthread_detach(pthread_self());
    FuncData *func_data = reinterpret_cast<FuncData *>(m);
    int linkage_fd = func_data->fd;
    int (*write_message)(int);
    write_message = func_data->write_func;
    int (*read_message)(int);
    read_message = func_data->read_func;
    
    printf("create thread,tid=%u, get linkage fd=%d\n",
            (unsigned int)pthread_self(), linkage_fd);

    int ret = write_message(linkage_fd);
    if (ret < 0) {
        printf("write_message error ret=%d.\n", ret);
    }
    ret = read_message(linkage_fd);
    if (ret < 0) {
        printf("read_message error ret=%d.\n", ret);
    }
    close(linkage_fd);
    return NULL;
}

int sample_listen(int &listening_fd)
{
    int ret;
    // create socket fd
    listening_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_fd <= 0 ) {
        printf ("when create socket fd, there is a wrong. errno:%d", listening_fd);
        return -1;
    }
    printf ("success to create socket fd [%d]\n", listening_fd);

    // set socketopt
    int reuseaddr = 1;
    ret = setsockopt(listening_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseaddr, sizeof(reuseaddr));
    if (ret < 0) {
        printf("When setsockopt reuseaddr, there is a wrong, errno:%d,%s\n", errno,strerror(errno));
        return -1;
    }

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
    return 0;
}

/*
 * write_msg & read_msg is a sample protocol to transfer message
 * */
int write_char(int fd)
{
    char buffer[100] = "How are you?";
    ssize_t count = write(fd, buffer, strlen(buffer));
    printf("write [%s] to fd[%d], return [%d].\n", buffer, fd, count);
    return count;
}
int read_char(int fd)
{
    char buffer[100];
    ssize_t count = read(fd, buffer, 100);
    printf("read [%s] to fd[%d], return [%d].\n", buffer, fd, count);
    return count;
}

int one_accept(int &listening_fd, int (*write_message)(int), int (*read_message)(int))
{
    // accept
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


    int ret = write_message(linkage_fd);
    if (ret < 0) {
        printf("write_message error ret=%d.\n", ret);
    }
    ret = read_message(linkage_fd);
    if (ret < 0) {
        printf("read_message error ret=%d.\n", ret);
    }

    close(linkage_fd);

    // close listening fd
    close(listening_fd);
    return 0;
}

int thread_accept(int &listening_fd, int (*write_message)(int), int (*read_message)(int))
{
    while(1) {
        // accept
        int linkage_fd;
        sockaddr_in client_addr;
        bzero(&client_addr, sizeof(client_addr));
        int client_addr_len = sizeof(client_addr);
        linkage_fd = accept(listening_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
        if (linkage_fd < 0) {
            printf ("When accept a linkage, there is a wrong. errno:%d\n", linkage_fd);
            return 0;
        }
        printf ("accept a linkage [%d], ip[%s], port[%d]\n",
                linkage_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        FuncData func_data;
        func_data.fd = linkage_fd;
        func_data.write_func = write_message;
        func_data.read_func = read_message;
        // create thread
        pthread_t tid;
        int ret = pthread_create(&tid, NULL, pthread_func, &func_data);
        if(ret !=0) {
            printf("error\n");
            return 0;
        }

    }
    close(listening_fd);
    return 0;
}

TEST(CHAR, ONE)
{
    int listening_fd;
    int ret = sample_listen(listening_fd);
    EXPECT_EQ(ret, 0);
    ret = one_accept(listening_fd, write_char, read_char);
    EXPECT_EQ(ret, 0);
}

TEST(CHAR, THREAD)
{
    int listening_fd;
    int ret = sample_listen(listening_fd);
    EXPECT_EQ(ret, 0);
    ret = thread_accept(listening_fd, write_char, read_char);
    EXPECT_EQ(ret, 0);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
