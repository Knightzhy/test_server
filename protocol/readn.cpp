#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

ssize_t readn(int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    void *ptr;
    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR 
             || errno == EWOULDBLOCK 
             || errno == EAGAIN) {
                break;
            }

            printf("error, return nread=%d, errno=%d. error=%s\n",
                    nread, errno, strerror(errno));
            return -1;
        }
        printf("read xx=%d\n", nread);
        if (nread == 0) {
            break;
        }
        nleft -= nread;
        ptr += nread;
    }
    return n - nleft;
}
