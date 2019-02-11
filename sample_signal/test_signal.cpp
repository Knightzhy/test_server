#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <dlfcn.h>
#include "func.h"

void func1(int no)
{
    printf("yy\n");
    return;
}

typedef void (*CAC_FUNC)(int);
int main(int argc, char *argv[])
{
    while(1) {
        void (*f)(int);
        void *dl_handle = dlopen("libfunc.so", RTLD_LAZY);
        if (!dl_handle) {
            printf( "!!! %s\n", dlerror() );
            return 0;
        }
        f = (CAC_FUNC)dlsym(dl_handle, "func");
        char *err = dlerror();
        if (err != NULL) {
            printf( "!!! %s\n", err);
            return 0;
        }

        signal(SIGINT, f);
        signal(SIGFPE, f);
        if(dlclose(dl_handle)!=0) {
            printf("dlclose error.\n");
            return 0;
        }
        sleep(1);
    }
//    int b = 10 / 0;
    return 0;
}
