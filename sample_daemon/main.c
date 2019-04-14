#include <stdio.h>
#include "daemon.h"

int main()
{
    daemon_main();
    printf("Main: father %d, now process %d.\n", getppid(), getpid());
    return 0;
}
