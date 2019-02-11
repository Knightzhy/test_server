#include <stdio.h>
#include "func.h"
int count = 0;
void func(int no)
{
    if (count == 0) {
        printf("first.    ");
        count = 1;
    }
    printf("xx\n");
    return;
}
