#include <stdio.h>
#include <unistd.h>
ssize_t writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwriten;
    const void * ptr = vptr;
    nleft = n;
    while (nleft >0) {
        nwriten = write(fd, ptr, nleft);
        printf("write fd Bytes[%d]\n", nwriten);
        if (nwriten < 0) {
            return -1;
        }
        nleft -= nwriten;
        ptr += nwriten;
    }
    return n;
}
