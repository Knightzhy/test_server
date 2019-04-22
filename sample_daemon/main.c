#include <stdio.h>
#include "daemon.h"

int callback(int argc, char *argv[])
{
    printf("haha");
    return 99;
}

int main(int argc, char *argv[])
{
    daemon_configure_t c;
    c.callback = callback;
    c.full_name = "haha";
    c.version = "v1.0";
    c.author = "knight";
    c.mail = "xx@qq.mail";
    daemon_main(&c, argc, argv);
    //printf("Main: father %d, now process %d.\n", getppid(), getpid());
    return 0;
}
