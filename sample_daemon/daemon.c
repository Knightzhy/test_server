#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int daemon_main()
{
    printf("Z father %d, now process %d.\n", getppid(), getpid());
#ifdef DEBUG
    sleep(5);
#endif
    int pid = fork();
    if (pid < 0) {
        printf("fork error:%d\n", pid);
        return -1;
    }

    if (pid > 0) {
        printf("A father %d exit, has children %d.\n", getpid(), pid);
        exit(0);
    }

    int session_id = setsid();
    if (session_id < 0) {
        printf("setsid error:%d.\n", session_id);
    }
    printf("B session_id=%d, pid=%d, father %d.\n", session_id, getpid(), getppid());
#ifdef DEBUG
    sleep(5);
#endif

    pid = fork();
    if (pid < 0) {
        printf("fork error:%d\n", pid);
        return -1;
    }

    if (pid > 0) {
        printf("A father %d exit, has children %d.\n", getpid(), pid);
        exit(0);
    }
    printf("D session_id=%d, pid=%d, father %d.\n", session_id, getpid(), getppid());
#ifdef DEBUG
    sleep(5);
#endif

    umask(0);

    int i;
    for (i = 3; i<getdtablesize(); i++) {
        close(i);
    }

    i = open("test.txt", O_RDWR | O_CREAT, 0600);
    if (i<0) {
        printf("open error.\n");
    }
    dup2(i, 0);
    dup2(i, 1);
    dup2(i, 2);
    close(i);
    printf("test HAHAHA.\n");
}
