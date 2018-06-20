#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main ()
{
    int ret;
    // create socket fd
    int listening_fd;
    listening_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_fd <= 0 ) {
        printf ("when create socket fd, there is a wrong. errno:%d", listening_fd);
        return 0;
    }
    printf ("success to create socket fd [%d]\n", listening_fd);

    // set socketopt
    int reuseaddr = 1;
    ret = setsockopt(listening_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseaddr, sizeof(reuseaddr));
    if (ret < 0) {
        printf("When setsockopt reuseaddr, there is a wrong, errno:%d,%s\n", errno,strerror(errno));
        return 0;
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
        return 0;
    }
    printf ("bind success.\n");

    // listening
    if (listen(listening_fd, 5) < 0) {
        printf("When listen to the socket, there is a wrong.\n");
        return 0;
    }
    printf ("listen success\n");

    // create epoll
    int epoll_fd = epoll_create(1000);
    struct epoll_event ev;
    struct epoll_event evs[1000];
    ev.events= EPOLLIN | EPOLLET;
    ev.data.fd = listening_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listening_fd, &ev) < 0) {
        printf("epoll add listening_fd error, errno:%d\n", errno);
        return 0;
    }
    int wait_fds;
    int cur_fds = 1;
    while (true) {
        if ( (wait_fds = epoll_wait(epoll_fd, evs, cur_fds, -1) )== -1) {
            printf ("epoll wait error, %d, %s.\n", errno, strerror(errno));
            return 0;
        }
        for (int i=0; i< wait_fds; i++) {
            if (evs[i].data.fd == listening_fd &&cur_fds < 1000) {
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

                ev.events= EPOLLIN | EPOLLET;
                ev.data.fd = linkage_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, linkage_fd, &ev) < 0) {
                    printf("epoll add linkage_fd error, errno:%d\n", errno);
                    return 0;
                }

                cur_fds++;
                continue;
            }
            char buff[50];
            ssize_t count;
            count = read(evs[i].data.fd, &buff, 50);
            if (count<=0) {
                printf("read error, %d.\n", (int)count);
                close(evs[i].data.fd);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, evs[i].data.fd, &ev);
                cur_fds--;
                continue;
            }

            // write data
            char buff2[100] = "hahahahha";
            strncat(buff2, buff, sizeof(buff));
            count = write(evs[i].data.fd, &buff2, sizeof(buff2));
            printf("When write, count=%d.\n", (int)count);

        }
    }
    close(epoll_fd);

    // close listening fd
    close(listening_fd);
    return 0;
}
