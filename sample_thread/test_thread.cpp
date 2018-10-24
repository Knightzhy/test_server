#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string>
#include <stdlib.h>

struct member{
    char* name;
    uint16_t age;
};
void *func(void* m)
{
    sleep(2);
    printf("create thread id=%u\n", (uint32_t)pthread_self());
    member *a = (struct member*)m;
    printf("m.name=%s, m.age=%d\n", a->name, a->age);
    return NULL;
}


int main()
{
    pthread_t tid;
    int ret;
    member *a = (struct member *)malloc(sizeof(struct member));
    if (a==NULL){
        printf("malloc error\n");
        return 0;
    }
    a->name = "HH";
    a->age = 16;
    printf("m.name=%s, m.age=%d\n", a->name, a->age);
    ret = pthread_create(&tid, NULL, func, a);
    printf("ret=%d, thread id=%u\n", ret, (uint32_t)pthread_self());
    ret = pthread_join(tid, NULL);
    printf("join ret=%d\n", ret);
    free(a);
    return 0;
}
