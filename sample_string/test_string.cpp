#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <gtest/gtest.h>

/*
 *
    char a[] = "ABC";
    char b[10];
    char c[1];
    char *d;
    char *e[];
    char f[][];
    char *g[][];
    const char *
    char * const
    static char *
    void *
    int *
 * */

TEST(ST, A)
{
    char a[] = "ABC";
    printf("str=%s, length=%u, sizeof=%u\n",
            a, (unsigned int)strlen(a), (unsigned int)sizeof(a));
    a[1] = 'M';
    printf("str=%s\n", a);
    strcpy(a, "NNNMMM"); // Segmentation fault
    // a = "DDD"; error, invalid array assignment
    printf("str=%s\n", a);
    printf("HAHA\n");
}

TEST(ST, B)
{
    char a[1];
    a[0] = 'a';
    a[1] = 'b';
    std::cout<<"One Step:a[0] = "<<a[0]<<"    a[1] = "<<a[1]<<std::endl;
    std::cout << "One Step:a=" << a << std::endl;
    std::cout << "One Step:a.length=" << strlen(a)
        << "    a.sizeof=" << sizeof(a) << std::endl;

    char b[1];
    b[0] = 'c';
    b[1] = 'd';
    std::cout<<"Two Step:a[0] = "<<a[0]<<"    a[1] = "<<a[1]<<std::endl;
    std::cout << "Two Step:a=" << a << std::endl;
    std::cout << "Two Step:a.length=" << strlen(a)
        << "    a.sizeof=" << sizeof(a) << std::endl;
    std::cout<<"Two Step:b[0] = "<<b[0]<<"    b[1] = "<<b[1]<<std::endl;
    std::cout << "Two Step:b=" << b << std::endl;
    std::cout << "Two Step:b.length=" << strlen(b)
        << "    b.sizeof=" << sizeof(b) << std::endl;

    strcpy(a, b);// cover a
    //strcpy(b, a);

    std::cout<<"Third Step:a[0] = "<<a[0]<<"    a[1] = "<<a[1]<<std::endl;
    std::cout << "Third Step:a=" << a << std::endl;
    std::cout << "Third Step:a.length=" << strlen(a)
        << "    a.sizeof=" << sizeof(a) << std::endl;
    std::cout<<"Third Step:b[0] = "<<b[0]<<"    b[1] = "<<b[1]<<std::endl;
    std::cout << "Third Step:b=" << b <<std::endl;
    std::cout << "Third Step:b.length=" << strlen(b)
        << "    b.sizeof=" << sizeof(b) << std::endl;

    printf("ThirdStep a::%x\n",a);
    printf("ThirdStep b:%x\n",b);

    /*
     * when strcpy(a, b) between Step2 and Step3.
     *
     *    stacks  Step1     Step2     Step3
     *       b0              c         c
     *    b1 a0    a         d         c
     *       a1    b         b         d
     *             /0        /0        b
     *                                 /0
     *
     * when strcpy(b, a) between Step2 and Step3.
     *
     *    stacks  Step1     Step2     Step3
     *       b0              c         d
     *    b1 a0    a         d         b
     *       a1    b         b         /0
     *             /0        /0        /0
     * */
}

TEST(ST, C)
{
    char a[1];
    char b[1];
    printf("a addr=%p\n", &a);
    printf("b addr=%p\n", &b[1]);
}

TEST(ST, D)
{
    char a[2] = "A";
    // char a[1] = "A"; error
    
    /*
     * storage size of ‘b’ isn’t known
     * char b[];
     * b = "ABC";
     * */
    strcpy(a, "HAHS");
    printf("a=%s\n", a);
}

TEST(ST, E)
{
    char *a = "ABC";
    // a[1] = 'E'; // Segmentation fault
    std::cout << "a.length" << strlen(a)
        << "    a.sizeof=" << sizeof(a)
        << "    a[0]=" << a[0]
        << "    a[0]+1=" << a[0]+1
        << "    (char)(a[0]+1)=" << (char)(a[0]+1)
        << "    &a[0]=" << &a[1]
        << "    a+1=" << a+1
        << std::endl;
    printf("%p    %p    %p\n", a, &a[1], &a);
    a = (char *)malloc(sizeof(char) * 50);
    strcpy(a, "Hello World,Yes");
    std::cout << a << std::endl;
    free(a);
    a = NULL;
}

TEST(ST, F)
{
    char *a[10];
    memset(a, 0, 10);
    printf("%p\n", a);
    a[0] = "ANCBs";
    printf("%p   %p   %s\n", &a[0], a[0], a[0]);
}

TEST(ST, G)
{
    char a[4][5] = {
                    {'a','b','c','d','e'},
                    {'f','g','h','i','j'},
                    {'k','l','m','n','o'},
                    {'p','q','r','s','t'},
                    };
    for (int i=0; i<4; i++) {
        for (int j=0; j<5; j++) {
            printf("%c\t", a[i][j]);
        }
        printf("\n");
    }
}

TEST(ST, H)
{
    char *a[4][5] = {
                    {"aA","bB","cC","dD","eE"},
                    {"fF","gG","hH","iI","jJ"},
                    {"kK","lL","mM","nN","oO"},
                    {"pP","qQ","rR","sS","tT"},
                    };
    for (int i=0; i<4; i++) {
        for (int j=0; j<5; j++) {
            printf("%s\t", a[i][j]);
        }
        printf("\n");
    }
}
TEST(ST, I)
{
    const char *a;
    a = "AXS";
    printf("a=%s\n", a);
    a = "BBB";
    printf("a=%s\n", a);
    char b[] = "KKKK";
    a = b;
    printf("a=%s, b=%s\n", a, b);

    /*
     * assignment of read-only location ‘*(a + 1u)’
     *
     * a[1] = 'M';
     * printf("a=%s\n", a);
    */

    char *c;
    c = b;
    c[1] = 'M';
    printf("a=%s, b=%s, c=%s\n", a, b, c);

    /*
     * assignment of read-only variable ‘d’
     *
     * char * const d;
     * d = b;
     * */

    char * const d = b;
    d[2] = 'L';
    printf("a=%s, b=%s, c=%s, d=%s\n", a, b, c, d);
}

static char *a;

TEST(ST, J)
{
    a = "ABC";
    printf("a=%s\n", a);
}

TEST(ST, K)
{
    printf("a=%s\n", a);
}

TEST(ST, L)
{
    char a[4][5] = {
                    {'a','b','c','d','e'},
                    {'f','g','h','i','j'},
                    {'k','l','m','n','o'},
                    {'p','q','r','s','t'},
                    };
    printf("a point=%p\n", a);
    for (int i=0; i<4; i++) {
        printf("a[%d] point=%p\n", i, &a[i]);
        for (int j=0; j<5; j++) {
            printf("a[%d][%d] point=%p\n", i, j, &a[i][j]);
        }
        printf("\n");
    }
}

TEST(ST, M)
{
    char * a;
    a = "ABCND";
    printf("a=%s\tpoint=%p\n", a, a);
    void *b = (void *)a;
    printf("b=%s\tpoint=%p\n", b, b);
    int *c = (int *)a;
    printf("c=%s\tpoint=%p\n", c, c);
    uint32_t *d = (uint32_t *)a;
    printf("d=%s\tpoint=%p\n", d, d);
    uint16_t *e = (uint16_t *)a;
    printf("e=%s\tpoint=%p\n", e, e);
    uint8_t *f = (uint8_t *)a;
    printf("f=%s\tpoint=%p\n", f, f);
    uint64_t g = (uint64_t)a;
    printf("g=%s\tpoint=%p\n", (char *)g, (char *)g);
    void * h = (void *)a;
    printf("h=%s\tpoint=%p\n", h, h);
}

TEST(ST, N)
{
#pragma pack(push)
#pragma pack(push)
    struct Message{
        uint32_t length;
        char msg[1];
    };
#pragma pack(pop)
    printf("Message sizeof=%d\n", sizeof(Message));
    void * buffer = (void *)malloc(18);
    char * buffer_char = reinterpret_cast<char *>(buffer);
    Message *a = reinterpret_cast<Message *>(buffer);
    a->length = 10;
    strcpy(a->msg, "ABCDE");
    Message *b = reinterpret_cast<Message *>(buffer + 10);
    b->length = 8;
    strcpy(b->msg, "FGH");

    printf("buffer_char point\n");
    for(int i =0; i < 18; i++){
        printf("%x\t%p\n", (char)buffer_char[i], &buffer_char[i]);
    }

    printf("a.point=%p\n", a);
    printf("b.point=%p\n", b);

    Message *c = reinterpret_cast<Message *>(buffer);
    printf("c.point=%p,c.length=%d, c.msg=%s, c.msg.length=%d\n",
            c, c->length, c->msg, strlen(c->msg));
    Message *d = reinterpret_cast<Message *>(buffer + 10);
    printf("d.point=%p,d.length=%d, d.msg=%s, d.msg.length=%d\n",
            d, d->length, d->msg, strlen(d->msg));
    free(buffer);
}

TEST(ST, O)
{
#pragma pack(push)
#pragma pack(push)
    struct Message{
        char name[4];
        char msg[1];
    };
#pragma pack(pop)
    printf("Message sizeof=%d\n", sizeof(Message));
    void * buffer = (void *)malloc(18);
    char * buffer_char = reinterpret_cast<char *>(buffer);
    Message *a = reinterpret_cast<Message *>(buffer);
    strcpy(a->name, "aaa");
    strcpy(a->msg, "ABCDE");
    Message *b = reinterpret_cast<Message *>(buffer + 10);
    strcpy(b->name, "bbb");
    strcpy(b->msg, "FGH");

    printf("buffer_char point\n");
    for(int i =0; i < 18; i++){
        printf("%x\t%p\n", (char)buffer_char[i], &buffer_char[i]);
    }

    printf("a.point=%p\n", a);
    printf("b.point=%p\n", b);

    Message *c = reinterpret_cast<Message *>(buffer);
    printf("c.point=%p,c.name=%s, c.msg=%s\n", c, c->name, c->msg);
    Message *d = reinterpret_cast<Message *>(buffer + 10);
    printf("d.point=%p,d.name=%s, d.msg=%s\n", d, d->name, d->msg);
    free(buffer);
}

TEST(ST, P)
{
    void * buffer1 = (void *)malloc(10);
    void * buffer2 = (void *)malloc(8);
    printf("buffer1.point=%p\n", buffer1);
    printf("buffer2.point=%p\n", buffer2);
    free(buffer1);
    free(buffer2);
}

TEST(ST, Q)
{
    std::string msg = "AN\0MMjseOOqQKW";
    std::cout << "msg:" << msg << std::endl;
    std::cout << "length:" << strlen(msg.c_str())
              << ", sizeof=" << sizeof(msg)
              << ", size()=" << msg.size()
              << ", msg[5]=" << msg[5]
              << std::endl;
}

TEST(ST, R)
{
struct msg{
    char payload[100];
};
    msg a;
    printf("sizeof=%d, sizeof=%d, strlen=%d\n", sizeof(msg), sizeof(a), strlen(a.payload));
}

TEST(ST, S)
{
    int a[10];
    memset(a, -1, 10);
    for (int i = 0; i < 10; i++) {
        printf("i:%d\t", a[i]);
    }
}


int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
