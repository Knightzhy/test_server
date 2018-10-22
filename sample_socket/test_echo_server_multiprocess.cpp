#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
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
void echo(int linkage_fd)
{
    // write data
    char buff[50];
    ssize_t count = read(linkage_fd, &buff, 50);
    printf("When read, count=%d, buff=%s.\n", (int)count, buff);

    char buff2[50] = "MMMMMMMJJJ";
    count = write(linkage_fd, buff2, strlen(buff2));
    printf("When write, count=%d.\n", (int)count);
}

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
    printf ("listen success.\n");

    while (1) {
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

        pid_t pid;
        pid = fork();
        if (pid <0) {
            printf("fork error.\n");
            return 0;
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
            echo(linkage_fd);
            close(linkage_fd);
            exit(0);
        }
    }

    // close listening fd
    close(listening_fd);
    return 0;
}
