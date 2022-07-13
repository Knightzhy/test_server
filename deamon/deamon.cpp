#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <signal.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

void callback()
{
    sleep(10);
}

int deamon()
{
    pid_t pid ;
    //signal(SIGCHLD,SIG_IGN);
    pid = fork();
    if(pid == -1)
    {
        perror("tile");
        return -1;
    }
    if(pid > 0)           //父进程先退出
    {
        int status;
        wait(&status);
    }
    else if(pid == 0){  //退出,被托付给init进程
        pid_t pid2;
        pid2 = fork();
        if (pid2 == -1) {
            perror("tile");
            return -1;
        }
        if (pid2 > 0) {
            exit(0);
        } else if (pid2 == 0) {
            pid_t sid = setsid();

            if(chdir("/") < 0){
                perror("chdir error");
                exit(1);
            }
            //step4：重设文件权限掩码
            umask(0);
            //关闭文件描述符
            close(0);
            int t = open("/dev/null",O_RDWR);
            dup2(0,1);//使用重定向函数将
            dup2(0,2);
            callback();
            exit(0);
        }
    }
}

int main(void)
{
    deamon();
}
