#include <fcntl.h>
#include <stdlib.h>
#include <gtest/gtest.h>

#include <unistd.h>
// ssize_t write(int fd, const void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n)
{
    size_t nleft = n;
    const void * nptr = vptr;
    ssize_t nwriten;
    while(nleft >0) {
        printf("writen.n=%s, nleft=%d.\n", nptr, (int)nleft);
        nwriten = write(fd, nptr, nleft);
        printf("writen.n=%d.\n", (int)nwriten);
        if (nwriten <=0) {
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
    int fd = open("aa", O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    ASSERT_GT(fd, 0);
    printf("fd=%d.\n", fd);
    char v[] = "sdkqoweiqsdsfa";
    ssize_t n = writen(fd, v, strlen(v));
    printf("n=%d.\n", (int)n);

    close(fd);
}

// ssize_t read(int fd, void *buf, size_t nbytes);



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
