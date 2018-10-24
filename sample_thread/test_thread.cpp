#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
void *func(void*)
{
    printf("create thread id=%u\n", (uint32_t)pthread_self());
    return NULL;
}


int main()
{
    pthread_t tid;
    int ret;
    ret = pthread_create(&tid, NULL, func, NULL);
    printf("ret=%d, thread id=%u\n", ret, (uint32_t)pthread_self());
    ret = pthread_join(tid, NULL);
    printf("join ret=%d\n", ret);
    return 0;
}
