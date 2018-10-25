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
    strcpy(a, "NNNMMM");
    // a = "DDD"; error, invalid array assignment
    printf("str=%s\n", a);
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
    char *a;
    a = "ABC";
    std::cout << "a.length" << strlen(a)
        << "    a.sizeof=" << sizeof(a)
        << "    a[0]=" << a[0]
        << "    a[0]+1=" << a[0]+1
        << "    (char)(a[0]+1)=" << (char)(a[0]+1)
        << "    &a[0]=" << &a[1]
        << "    a+1=" << a+1
        << std::endl;
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
