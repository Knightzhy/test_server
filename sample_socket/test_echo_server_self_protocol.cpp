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

#include "test_server/protocol/rpc.h"

void echo(int linkage_fd)
{
    void *buffer = (void *)malloc(1024);
    ssize_t count = read(linkage_fd, buffer, 1024);
    std::string msg = rpc::Rpc::Parse(buffer, (size_t)count);
    printf("When read, count=%d, msg=%s.\n", (int)count, msg.c_str());

    msg = "Yes, This is my World.";
    size_t length = rpc::Rpc::GetMessageLength(msg);
    printf("length=%d\n", (int)length);
    buffer = realloc(buffer, (unsigned int)length);
    memset(buffer, 0, length);
    rpc::Rpc::Serialize(buffer, msg);
    count = write(linkage_fd, buffer, length);
    printf("count=%d\n", (int)count);

    free(buffer);
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
    printf ("listen success]n");

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

    echo(linkage_fd);

    close(linkage_fd);

    // close listening fd
    close(listening_fd);
    return 0;
}
