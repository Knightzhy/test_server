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
#include "test_server/protocol/rpc.h"
extern ssize_t writen(int fd, const void *vptr, size_t n);
extern ssize_t readn(int fd, void *vptr, size_t n);

/*
 * continuous packet with user_defined protocol
 * */
size_t GetMessageLength(const std::vector<std::string> &v)
{
    if (v.size() == 0) {
        return 0;
    }
    size_t length = 0;
    for(int i=0; i < v.size(); i++) {
        length += rpc::Rpc::GetMessageLength(v[i]);
    }
    return length;
}
ssize_t write_string2(int fd)
{
    std::vector<std::string> v;
    v.push_back("ABC");
    v.push_back("ABC1L");
    v.push_back("ABC2N");
    v.push_back("ABC3M");

    size_t total_length = GetMessageLength(v);
    printf("total_length=%d\n", total_length);
    void *buffer = (void *)malloc(total_length);
    size_t length = 0;
    for (int i=0; i < v.size() ; i++) {
        length = length + rpc::Rpc::Serialize((buffer + length), v[i]);
        if (total_length < length) {
            printf("Error, total_length=%d, length=%d\n",
                    total_length, length);
            return 0;
        }
    }
    if (total_length != length) {
        printf("Error, total_length=%d, length=%d\n",
                total_length, length);
        return 0;
    }

    ssize_t count = writen(fd, buffer, total_length);
    printf("When writen, count=%d\n", count);
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
        printf("Read msg:%s\n", msg.c_str());
        buffer_used = buffer_used + msg_length;
        count_used = count_used-msg_length;
    } while(count_used > 0);
    return count_used;
}

ssize_t read_string2(int fd)
{
    size_t buffer_length = 1024;
    void *buffer = (void *)malloc(buffer_length);
    void *buffer_used;
    ssize_t count_used = 0;

    do {
        buffer_used = buffer;
        memset(buffer_used + count_used, 0, buffer_length - count_used);
        ssize_t count;
        printf("Readn, need Bytes[%d]\n",(int)(buffer_length-count_used));
        count = readn(fd, buffer_used + count_used, buffer_length - count_used);
        if (count == 0) {
            printf("read bytes=0\n");
            break;
        }
        if (count < 0) {
            printf("read error, ret=%d\n", (int)count);
            break;
        }
        count_used = count_used + count;
        if (count_used > buffer_length) {
            printf("Error, count>buffer_length\n");
        }

        count_used = parse(buffer_used, count_used);
        strncpy((char *)buffer, (char *)buffer_used, count_used);
    } while(count_used != 0);

    free(buffer);
}

void echo(int linkage_fd)
{
    read_string2(linkage_fd);
    write_string2(linkage_fd);
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
