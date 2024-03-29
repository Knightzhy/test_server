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

#include "test_server/protocol/rpc.h"
#include "ironman/serialize/sample_header.h"
#include "ironman/serialize/string_payload.h"
#include "ironman/serialize/message.h"
#include "ironman/serialize/message_factory.h"
#include "ironman/serialize/rpc.h"
#include "ironman/serialize/safe_io.h"
#pragma pack(push)
#pragma pack(1)
struct FuncData {
    int fd;
    int (*write_func)(int);
    int (*read_func)(int);
};
#pragma pack(pop)


int sample_listen(int &listening_fd);

int select_accept(int &listening_fd, int (*write_message)(int), int (*read_message)(int));
int one_accept(int &listening_fd, int (*write_message)(int), int (*read_message)(int));
int thread_accept(int &listening_fd, int (*write_message)(int), int (*read_message)(int));
int process_accept(int &listening_fd, int (*write_message)(int), int (*read_message)(int));

int write_char(int fd);
int read_char(int fd);

int write_string(int fd);
int read_string(int fd);

int write_message(int fd);
int read_message(int fd);

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
    server_addr.sin_port = htons(9922);
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

void add_fd(int fd, int array_fd[], int size)
{
    for (int i = 0; i < size ; i++) {
        if (array_fd[i] <= 0) {
            array_fd[i] = fd;
            break;
        }
    }
}
void del_fd(int fd, int array_fd[], int size)
{
    for (int i = 0; i < size; i++) {
        if (array_fd[i] == fd) {
            array_fd[i] = -1;
            break;
        }
    }
}

int one_accept(int &listening_fd)
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

    return linkage_fd;
}

int select_accept(int &listening_fd, int (*write_message)(int), int (*read_message)(int))
{
    int max_fd = listening_fd;
    fd_set rfds;
    fd_set wfds;
    int array_linkage_fd[1024];
    for (int i = 0; i < 1024; i++) {
        array_linkage_fd[i] = -1;
    }
    
    while (1) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);

        // Add listening_fd
        FD_SET(listening_fd, &rfds);

        // Add array_linkage_fd
        for (int i = 0; i < 1024; i++) {
            if (array_linkage_fd[i] <= 0) {
                continue;
            }
            FD_SET(array_linkage_fd[i], &rfds);
            FD_SET(array_linkage_fd[i], &wfds);
            if (array_linkage_fd[i] > max_fd) {
                max_fd = array_linkage_fd[i];
            }
        }

        int ret = select(max_fd + 1, &rfds, &wfds, NULL, NULL);
        printf("SELECT return [%d].\n", ret);
        if (ret == 0 || ret == -1) {
            break;
        }

        // FD_ISSET listening_fd
        if (FD_ISSET(listening_fd, &rfds)) {
            int linkage_fd = one_accept(listening_fd);
            add_fd(linkage_fd, array_linkage_fd, 1024);
        }
        
        // FD_ISSET array_linkage_fd
        for (int i = 0; i < 1024; i++) {
            if (!FD_ISSET(array_linkage_fd[i], &rfds)) {
                continue;
            }
            ret = read_message(array_linkage_fd[i]);
            if (ret < 0) {
                printf("read_message error ret=%d.\n", ret);
            }
        }
        for (int i = 0; i < 1024; i++) {
            if (!FD_ISSET(array_linkage_fd[i], &wfds)) {
                continue;
            }
            int ret = write_message(array_linkage_fd[i]);
            if (ret < 0) {
                printf("write_message error ret=%d.\n", ret);
            }
        }
    }
    close(listening_fd);
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

/*
 * single packet with user-defined protocol
 * */
int write_string(int fd)
{
    std::string msg = "What's wrong?";
    size_t length = rpc::Rpc::GetMessageLength(msg);
    void *buffer = (void *)malloc(length);
    size_t length2 = rpc::Rpc::Serialize(buffer, msg);
    ssize_t count = ironman::serialize::writen(fd, buffer, length2);
    printf("write [%s] to fd[%d] "
            "GetMessageLength.length=%d, Serialize.length=%d, writen.length=%d\n",
            msg.c_str(), fd,
            (int)length, (int)length2, (int)count);
    free(buffer);
    buffer = NULL;
    return count;
}

ssize_t parse(void *buffer_used, ssize_t count_used)
{
    do{
        ssize_t msg_length = rpc::Rpc::GetMessageLength(buffer_used,
                (size_t)count_used);
        if (msg_length == 0) {
            break;
        }
        std::string msg;
        int ret = rpc::Rpc::Parse(buffer_used, msg_length, msg);
        printf("read [%s]\n", msg.c_str());
        buffer_used = buffer_used + msg_length;
        count_used = count_used-msg_length;
    } while(count_used > 0);
    return count_used;
}

int read_string(int fd)
{
    size_t buffer_length = 1024;
    void *buffer = (void *)malloc(buffer_length);
    void *buffer_used;
    ssize_t count_used = 0;

    ssize_t total_length = 0;
    do {
        buffer_used = buffer;
        memset(buffer_used + count_used, 0, buffer_length - count_used);
        ssize_t count;
        count = ironman::serialize::readn(fd, buffer_used + count_used, buffer_length - count_used);
        if (count == 0) {
            break;
        }
        if (count < 0) {
            printf("readn error, ret=%d\n", (int)count);
            free(buffer);
            return -1;
        }
        count_used = count_used + count;
        if (count_used > buffer_length) {
            printf("Error, count>buffer_length %d > %d\n", count_used, buffer_length);
            free(buffer);
            return -1;
        }

        count_used = parse(buffer_used, count_used);
        strncpy((char *)buffer, (char *)buffer_used, count_used);
        total_length += count_used;
    } while(count_used != 0);

    free(buffer);
    return total_length;
}

/*
 * user define protocol
 * */
int write_message(int fd)
{
    ironman::serialize::StringPayload string_payload("Yes, it's me.");
    ironman::serialize::Message message(98120, &string_payload);
    ironman::serialize::rpc::MessageFactory message_factory(&message);
    ironman::serialize::rpc::RpcBase rpc_base;
    int ret = rpc_base.Sended(fd, &message_factory);
    printf("Sended ret=%d.\n", ret);
    string_payload.PrintMsg();
    return ret;
}
int read_message(int fd)
{
    ironman::serialize::StringPayload string_payload;
    ironman::serialize::Message message(98120, &string_payload);
    ironman::serialize::rpc::MessageFactory message_factory(&message);
    ironman::serialize::rpc::RpcBase rpc_base;
    int ret = rpc_base.Received(fd, &message_factory);
    printf("Received ret=%d.\n", ret);
    return ret;
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

int process_accept(int &listening_fd, int (*write_message)(int), int (*read_message)(int))
{
    while (1) {
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

        pid_t pid;
        pid = fork();
        if (pid <0) {
            printf("fork error.\n");
            return -1;
        }
        if (pid >0) {
            waitpid(pid, NULL, 0);
            printf("grand father pid=%d, child pid=%d\n", (int)getpid(), (int)pid);
            close(linkage_fd);
            continue;
        }

        printf("child pid=%d\n", (int)getpid());
        close(listening_fd);
        pid =fork();
        if (pid <0) {
            close(linkage_fd);
            exit(1);
        }
        if (pid >0) {
            printf("child pid=%d, grand son pid=%d\n", (int)getpid(), (int)pid);
            close(linkage_fd);
            exit(0);
        } else {
            printf("grand son pid=%d\n", (int)getpid());
            FuncData func_data;
            func_data.fd = linkage_fd;
            func_data.write_func = write_message;
            func_data.read_func = read_message;
            pthread_func(&func_data);
            close(linkage_fd);
            exit(0);
        }
    }

    // close listening fd
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

TEST(CHAR, PROCESS)
{
    int listening_fd;
    int ret = sample_listen(listening_fd);
    EXPECT_EQ(ret, 0);
    ret = process_accept(listening_fd, write_char, read_char);
    EXPECT_EQ(ret, 0);
}

TEST(STRING, ONE)
{
    int listening_fd;
    int ret = sample_listen(listening_fd);
    EXPECT_EQ(ret, 0);
    ret = one_accept(listening_fd, write_string, read_string);
    EXPECT_EQ(ret, 0);
}

TEST(STRING, THREAD)
{
    int listening_fd;
    int ret = sample_listen(listening_fd);
    EXPECT_EQ(ret, 0);
    ret = thread_accept(listening_fd, write_string, read_string);
    EXPECT_EQ(ret, 0);
}

TEST(STRING, PROCESS)
{
    int listening_fd;
    int ret = sample_listen(listening_fd);
    EXPECT_EQ(ret, 0);
    ret = process_accept(listening_fd, write_string, read_string);
    EXPECT_EQ(ret, 0);
}

TEST(MESSAGE, ONE)
{
    int listening_fd;
    int ret = sample_listen(listening_fd);
    EXPECT_EQ(ret, 0);
    ret = one_accept(listening_fd, write_message, read_message);
    EXPECT_EQ(ret, 0);
}

TEST(MESSAGE, THREAD)
{
    int listening_fd;
    int ret = sample_listen(listening_fd);
    EXPECT_EQ(ret, 0);
    ret = thread_accept(listening_fd, write_message, read_message);
    EXPECT_EQ(ret, 0);
}

TEST(MESSAGE, PROCESS)
{
    int listening_fd;
    int ret = sample_listen(listening_fd);
    EXPECT_EQ(ret, 0);
    ret = process_accept(listening_fd, write_message, read_message);
    EXPECT_EQ(ret, 0);
}

// SELECT
TEST(CHAR, SELECT)
{
    int listening_fd;
    int ret = sample_listen(listening_fd);
    EXPECT_EQ(ret, 0);
    ret = select_accept(listening_fd, write_char, read_char);
    EXPECT_EQ(ret, 0);
}
// POLL
// EPOLL
// LIBEV
// LIBEVENT
// LIBUV

int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
