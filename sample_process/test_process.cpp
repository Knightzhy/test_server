#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
    printf("This is a process, pid=%d, parent id=%d\n",
            getpid(), getppid());
    pid = fork();
    if (pid <0) {
        printf("error");
        return 0;
    }
    if (pid == 0) {
        printf("This is a children process, pid=%d, parent id=%d\n",
                getpid(), getppid());
    }
    if (pid >0) {
        printf("This is a father process, pid=%d, parent id=%d\n",
                getpid(), getppid());
        pid_t cpid = wait(NULL);
        printf("This is a father parcess, pid=%d, my child is dead, his pid=%d\n",
                getpid(), cpid);
    }
    return 0;
}
