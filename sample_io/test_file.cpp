#include <fcntl.h>
#include <stdlib.h>
#include <gtest/gtest.h>

#include <unistd.h>
#include <error.h>
// ssize_t write(int fd, const void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n)
{
    printf("n=%d\n", n);
    size_t nleft = n;
    const void * nptr = vptr;
    ssize_t nwriten;
    while(nleft >0) {
        nwriten = write(fd, nptr, nleft);
        printf("writen.n=%d.\n", (int)nwriten);
        if (nwriten <=0) {
            printf("errno=%d,error=%s.\n", errno, strerror(errno));
            if (nwriten <0 && errno == EINTR) {
                nwriten = 0;
            } else {
                return -1;
            }
        }
        nleft -= nwriten;
        nptr += nwriten;
    }
    return n;
}

TEST(IOTest, TWriten)
{
    int fd = open("aa", O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR);
    ASSERT_GT(fd, 0);
    printf("fd=%d.\n", fd);
    char *v = new char[100000000];
    for (int i=0; i<100000001; i++) {
        v[i] = 'a';
    }
    v[100000000] = '\0';
    ssize_t n = writen(fd, v, strlen(v));
    ASSERT_EQ(n, strlen(v));
    delete []v;

    close(fd);
}

// ssize_t read(int fd, void *buf, size_t nbytes);
ssize_t readn(int fd, void *buf, size_t n)
{
}



TEST(IOTest, TCreat)
{
    int fd = creat("dd", S_IRUSR | S_IWUSR);
    ASSERT_GT(fd, 0);
    printf("fd=%d.\n", fd);
    close(fd);
}

TEST(IOTest, TOpen)
{
    int fd = open("qq", O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    ASSERT_GT(fd, 0);
    printf("fd=%d.\n", fd);
    close(fd);
}

int main(int argc, char*argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
