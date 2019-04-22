#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <getopt.h>
#include "daemon.h"

int daemon_print_help(const daemon_configure_t *configure)
{
    printf("help:xx\n");
}

int daemon_print_version(const daemon_configure_t *configure)
{
    printf("version:xx\n");
}

int daemon_print_license(const daemon_configure_t *configure)
{
    printf("license:xx\n");
}

int daemon_parse_argument(const daemon_configure_t *configure,
        int argc, char *argv[])
{
    static int lopt;
    static const char OPTIONS[] = "chk:v";
    static const struct option longOpts[] = {
        {"help",    no_argument, NULL, 'h'},
        {"license", no_argument, NULL, 'l'},
        {"version", no_argument, NULL, 'v'},
        {"ip", required_argument, &lopt, 12},
        {0,0,0,0}
    };

    int c;
    int option_index;
    while(1) {
        c = getopt_long(argc, argv, OPTIONS,
                        longOpts, &option_index);
        if (c < 0) break;
        if (c == 0) {
            printf("%s argument is %s\n", longOpts[option_index].name,
                    optarg);
            printf("%d\n", lopt);
            continue;
        }
        switch(c) {
            case 'c':
                printf("command: background.\n");
                break;
            case 'h':
                daemon_print_help(configure);
                return 1;
            case 'k':
                printf("command: %s.\n", optarg);
                break;
            case 'l':
                daemon_print_license(configure);
                return 1;
            case 'v':
                daemon_print_version(configure);
                return 1;
            case '?':
                return 1;
            default:
                ;
        };
    }
    return 1;
}

int daemon_main(const daemon_configure_t *configure, int argc, char *argv[])
{
    daemon_parse_argument(configure, argc, argv);
    return 1;
}

/*
int daemon_main(const daemon_configure_t *configure, int argc, char *argv[])
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
*/
