#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string>
#include <stdlib.h>

pthread_mutex_t m_mutex;

struct member{
    char* name;
    uint16_t age;
};
void *func(void* m)
{
    pthread_mutex_lock(&m_mutex);
    printf("child thread wait 8s\n");
    sleep(8);
    printf("create thread id=%u\n", (uint32_t)pthread_self());
    member *a = (struct member*)m;
    printf("m.name=%s, m.age=%d\n", a->name, a->age);
    pthread_mutex_unlock(&m_mutex);

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
    pthread_mutex_init(&m_mutex, NULL);
    ret = pthread_create(&tid, NULL, func, a);
    printf("ret=%d, thread id=%u\n", ret, (uint32_t)pthread_self());

    printf("main thread wait 4s\n");
    sleep(4);
    pthread_mutex_lock(&m_mutex);
    printf("main thread MMMM\n");
    pthread_mutex_unlock(&m_mutex);
    pthread_join(tid, NULL);
    free(a);
    pthread_mutex_destroy(&m_mutex);
    return 0;
}
