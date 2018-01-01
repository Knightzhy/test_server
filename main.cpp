#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
extern ssize_t writen(int fd, const void *vptr, size_t n);

void str_echo(int sockfd)
{
    ssize_t n;
    char buf[100];
    while ((n = read(sockfd, buf, 100)) >0) {
        writen(sockfd, buf, n);
    }
    if (n <0) {
        std::cout << "error:" << n << std::endl;
    }
}

int main(void)
{
    int listenfd;
    listenfd = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
    if (listenfd <= 0) {
        std::cout << "error when create socket: " << listenfd << std::endl;
        return 0;
    }
    std::cout << "listen fd:" << listenfd << std::endl;

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(19909);
    if (0 != bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr))) {
        std::cout << "bind error" << std::endl;
        return 0;
    }


    struct sockaddr_in cliaddr;
    socklen_t clilen;
    if (0 != listen(listenfd, 1)) {
        std::cout << "listen error" << std::endl;
        return 0;
    }

    int connfd;
    pid_t childpid;
    for (;;) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (sockaddr *)&cliaddr, &clilen);

        std::cout << "x:" << connfd <<std::endl;
        if ((childpid = fork()) == 0) {
            close(listenfd);
            str_echo(connfd);
            exit(0);
            close(connfd);
        }
        // TODO: close listenfd when get signal
        close(connfd);
    }


    return 0;
}
