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
#include <pthread.h>
#include <vector>

#include "ironman/serialize/sample_header.h"
#include "ironman/serialize/string_payload.h"
#include "ironman/serialize/message.h"
#include "ironman/serialize/message_factory.h"
#include "ironman/serialize/rpc.h"

int write_message(int fd)
{
    ironman::serialize::SampleHeader sample_header(56766, 1230, 118);
    ironman::serialize::StringPayload string_payload("Yes, it's me.");
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


void echo(int linkage_fd)
{
    read_message(linkage_fd);

    write_message(linkage_fd);
}

int set_nonblockint(int fd) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0))== -1){
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void *func(void* m)
{
    pthread_detach(pthread_self());
    int linkage_fd = *(int*)m;
    printf("create thread,tid=%u, get linkage fd=%d\n",
            (unsigned int)pthread_self(), linkage_fd);
    int ret = set_nonblockint(linkage_fd);
    printf("fcntl=%d\n", ret);
    echo(linkage_fd);
    sleep(5);
    close(linkage_fd);
    return NULL;
}

int main ()
{
    int ret;
    // create socket fd
    int listening_fd;
    listening_fd = socket(AF_INET, SOCK_STREAM , 0);
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

    while(1) {
        printf("accept\n");
        // accept
        int linkage_fd;
        sockaddr_in client_addr;
        bzero(&client_addr, sizeof(client_addr));
        int client_addr_len = sizeof(client_addr);
        linkage_fd = accept(listening_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
        if (linkage_fd < 0) {
            printf ("When accept a linkage, there is a wrong. errno:%d\n", linkage_fd);
            printf ("errno=%d, error=%s\n", errno, strerror(errno));
            return 0;
        }
        printf ("accept a linkage [%d], ip[%s], port[%d]\n",
                linkage_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // create thread
        pthread_t tid;
        int ret = pthread_create(&tid, NULL, func, &linkage_fd);
        if(ret !=0) {
            printf("error\n");
            return 0;
        }

    }
    close(listening_fd);
    return 0;

}
