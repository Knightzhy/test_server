#include <stdlib.h>
#include <string.h>
#include <malloc.h>

int main()
{

    char *a = (char *)malloc(4*sizeof(char));

    a = (char *)memset(a, 0, (4*sizeof(char)));
    a = (char *)memset(a, 'a', (4*sizeof(char)));
    printf("a:%s\n", a);

    char b[] = {"qqq"};

    strncat(a, b, 3);

    printf("a:%s\n", a);
    printf("a[4]%s\n", &(a[4]));
    printf("b:%s\n", b);

    free(a);
    a = NULL;

    return 0;
}
