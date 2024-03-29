#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <gtest/gtest.h>
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
#include <sys/poll.h>

#include "test_server/protocol/rpc.h"
#include "ironman/serialize/sample_header.h"
#include "ironman/serialize/string_payload.h"
#include "ironman/serialize/message.h"
#include "ironman/serialize/message_factory.h"
#include "ironman/serialize/rpc.h"
#include "ironman/serialize/safe_io.h"

 /* 
  * TODO
  * #include <sys/socket.h> 
  * int getsockname(int sockfd, struct sockaddr *localaddr, socklen_t *addrlen); 
  * int getpeername(int sockfd, struct sockaddr *peeraddr, socklen_t *addrlen);
  * */

int set_nonblockint(int fd) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0))== -1){
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int sample_connect(int &socket_fd)
{
    // create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0 ) {
        printf("When create socket, there is a wrong. errno:%d\n", socket_fd);
        return -1;
    }
    printf("create socket success.[%d]\n", socket_fd);

    // connet
    sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9922);
    inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);
    int ret = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        printf("When connect, there is a wrong. errno:%d\n", ret);
        return -1;
    }
    printf("connect success.\n");
    set_nonblockint(socket_fd);
    return 0;
}
/*
 * write_msg & read_msg is a sample protocol to transfer message
 * */
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

/*
 * single packet with user-defined protocol
 * */
int write_string(int fd)
{
    std::string msg = "Hello World.";
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
    ironman::serialize::StringPayload string_payload("Hello World,Tom.");
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

int epoll_client(int &socket_fd, int (*write_message)(int), int (*read_message)(int))
{
    int epoll_fd = epoll_create(5);
    struct epoll_event ev;
    struct epoll_event evs[10];
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = socket_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev) < 0) {
        printf("epoll add socket_fd error\n");
        close(epoll_fd);
        close(socket_fd);
        return -1;
   }
    int count = write_message(socket_fd);
    if (count < 0) {
        printf("write_message error ret=%d.\n", count);
        close(epoll_fd);
        close(socket_fd);
        return -1;
    }
    while(true) {
        int wait_fds = epoll_wait(epoll_fd, evs,1, -1);
        if (wait_fds <= -1) {
            printf("epoll wait error, errno=%d, error=%s\n", errno, strerror(errno));
            close(epoll_fd);
            close(socket_fd);
            return -1;
        }
        if (wait_fds == 0) {
            continue;
        }
        count = read_message(evs[0].data.fd);
        if (count < 0) {
            printf("read_message error ret=%d.\n", count);
            close(epoll_fd);
            close(socket_fd);
            return -1;
        }
        break;
    }

    close(epoll_fd);
    close(socket_fd);
    return 0;
}

/*
 * int select(int maxfdp,fd_set *readfds,fd_set *writefds,
 * fd_set *errorfds,struct timeval *timeout);
 * */
int select_client(int &socket_fd, int (*write_message)(int), int (*read_message)(int))
{
    int max_fd = socket_fd;
    fd_set rfds;
    fd_set wfds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    
    while (1) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_SET(max_fd, &rfds);
        FD_SET(max_fd, &wfds);
            
        int ret = select(max_fd + 1, &rfds, &wfds, NULL, &tv);
        printf("select return [%d]\n", ret);
        if (ret <=0) {
            printf("select return [%d]\n", ret);
            break;
        }
        if (FD_ISSET(max_fd, &rfds)) {
            int count = read_message(socket_fd);
            if (count < 0) {
                printf("read_message error ret=%d.\n", count);
                close(socket_fd);
                return -1;
            }
        }
        if (FD_ISSET(max_fd, &wfds)) {
            int count = write_message(socket_fd);
            if (count < 0) {
                printf("write_message error ret=%d.\n", count);
                close(socket_fd);
                return -1;
            }
        }
        sleep(1);
    }
}

/*
 * #include <poll.h>
 * int poll ( struct pollfd * fds, unsigned int nfds, int timeout);
 * struct pollfd {
 *     int fd;  
 *     short events; 
 *     short revents; 
 * };
 * */
int poll_client(int &socket_fd, int (*write_message)(int), int (*read_message)(int))
{
    pollfd pfd;
    pfd.fd = socket_fd;
    pfd.events = POLLIN;

    int ret = poll(&pfd, 1, 0);
    if (ret == -1){
        printf("poll error [%d]\n", ret);
        close(socket_fd);
        return -1;
    }
    while (1){
        if ((pfd.revents & POLLIN) == POLLIN) {
            int count = read_message(socket_fd);
            if (count < 0) {
                printf("read_message error ret=%d.\n", count);
                close(socket_fd);
                return -1;
            }
            count = write_message(socket_fd);
            if (count < 0) {
                printf("write_message error ret=%d.\n", count);
                close(socket_fd);
                return -1;
            }
            break;
        }
    }
    close(socket_fd);
    return 0;
}

TEST(CHAR, EPOLL)
{
    int socket_fd;
    int ret = sample_connect(socket_fd);
    EXPECT_EQ(ret, 0);
    ret = epoll_client(socket_fd, write_char, read_char);
    EXPECT_EQ(ret, 0);
}

TEST(STRING, EPOLL)
{
    int socket_fd;
    int ret = sample_connect(socket_fd);
    EXPECT_EQ(ret, 0);
    ret = epoll_client(socket_fd, write_string, read_string);
    EXPECT_EQ(ret, 0);
}

TEST(MESSAGE, EPOLL)
{
    int socket_fd;
    int ret = sample_connect(socket_fd);
    EXPECT_EQ(ret, 0);
    ret = epoll_client(socket_fd, write_message, read_message);
    EXPECT_EQ(ret, 0);
}

TEST(CHAR, SELECT)
{
    int socket_fd;
    int ret = sample_connect(socket_fd);
    EXPECT_EQ(ret, 0);
    ret = select_client(socket_fd, write_char, read_char);
    EXPECT_EQ(ret, 0);
}
TEST(CHAR, POLL)
{
    int socket_fd;
    int ret = sample_connect(socket_fd);
    EXPECT_EQ(ret, 0);
    ret = poll_client(socket_fd, write_char, read_char);
    EXPECT_EQ(ret, 0);
}

int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
