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
#include <vector>

#include "ironman/serialize/sample_header.h"
#include "ironman/serialize/string_payload.h"
#include "ironman/serialize/message.h"
#include "ironman/serialize/message_factory.h"
#include "ironman/serialize/rpc.h"

int write_message(int fd)
{
    ironman::serialize::SampleHeader sample_header(56766, 1230, 118);
    ironman::serialize::StringPayload string_payload("Hello World,Tom.");
    ironman::serialize::Message message(98120, &sample_header, &string_payload);
    ironman::serialize::rpc::MessageFactory message_factory(&message);
    ironman::serialize::rpc::RpcBase rpc_base;
    int ret = rpc_base.Sended(fd, &message_factory);
    printf("Sended ret=%d.\n", ret);
    sample_header.PrintOptions();
    string_payload.PrintMsg();
    return ret;
}
int read_message(int fd)
{
    ironman::serialize::SampleHeader sample_header;
    ironman::serialize::StringPayload string_payload;
    ironman::serialize::Message message(98120, &sample_header, &string_payload);
    ironman::serialize::rpc::MessageFactory message_factory(&message);
    ironman::serialize::rpc::RpcBase rpc_base;
    int ret = rpc_base.Received(fd, &message_factory);
    printf("Received ret=%d.\n", ret);
    return ret;
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
    set_nonblockint(socket_fd);
    int count = write_message(socket_fd);
    while(true) {
        printf("xx\n");
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
        count = read_message(evs[0].data.fd);
        break;
    }

    close(epoll_fd);
    close(socket_fd);
    return 0;
}
