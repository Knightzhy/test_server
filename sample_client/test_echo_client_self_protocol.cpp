#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "test_server/protocol/rpc.h"


ssize_t write_string(int fd)
{
    std::string msg = "Hello World.";
    size_t length = rpc::Rpc::GetMessageLength(msg);
    void *buffer = (void *)malloc(length);
    size_t length2 = rpc::Rpc::Serialize(buffer, msg);
    ssize_t count = write(fd, buffer, length2);
    printf("length=%d, length2=%d, count=%d\n",
            (int)length, (int)length2, (int)count);
    return count;
}

ssize_t read_string(int fd)
{
    void *buffer = (void *)malloc(1024);
    ssize_t count = read(fd, buffer, 1024);
    rpc::Message *message = reinterpret_cast<rpc::Message *>(buffer);
    printf("When read, count=%d, msg=%s, magic=%x, length=%d\n",
            (int)count, message->msg, message->magic, message->length);
    free(buffer);
    return count;
}

int set_nonblockint(int fd) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0))== -1){
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
int main()
{
    // create socket
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0 ) {
        printf("When create socket, there is a wrong. errno:%d\n", socket_fd);
        return 0;
    }
    printf("create socket success.[%d]\n", socket_fd);

    // connet
    sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(55555);
    inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);
    int ret = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        printf("When connect, there is a wrong. errno:%d\n", ret);
        return 0;
    }
    printf("connect success.\n");

    int epoll_fd = epoll_create(5);
    struct epoll_event ev;
    struct epoll_event evs[10];
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = socket_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev) < 0) {
        printf("epoll add socket_fd error\n");
        close(epoll_fd);
        close(socket_fd);
        return 0;
   }
    ssize_t count = write_string(socket_fd);
    while(true) {
        int wait_fds = epoll_wait(epoll_fd, evs,1, -1);
        if (wait_fds <= -1) {
            printf("epoll wait error, errno=%d, error=%s\n", errno, strerror(errno));
            close(epoll_fd);
            close(socket_fd);
            return 0;
        }
        if (wait_fds == 0) {
            continue;
        }
        count = read_string(evs[0].data.fd);
        break;
    }

    close(epoll_fd);
    close(socket_fd);
    return 0;
}