#define _GNU_SOURCE
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

int startShell(void *argument) {
    char* arguments[] = {"bash", NULL};
    char* environment[] = {NULL};
    execve("/bin/bash", arguments, environment);

    return 0;
}

int main() {
    mkdir(
        "/sys/fs/cgroup/my_group",
        S_IRWXO | S_IRGRP | S_IROTH
    );

    int stackSizeBytes = 65536;
    char *stackStart = malloc(stackSizeBytes);
    char *stackEnd = stackStart + stackSizeBytes;

    pid_t childPid = clone(
        startShell,
        stackEnd,
        SIGCHLD,
        NULL
    );

    FILE* procsFile = fopen(
        "/sys/fs/cgroup/my_group/cgroup.procs",
        "w"
    );

    fprintf(procsFile, "%d\n", childPid);
    fclose(procsFile);

    FILE *pidsMaxFile = fopen(
        "/sys/fs/cgroup/my_group/pids.max",
        "w"
    );

    fprintf(pidsMaxFile, "%d\n", 5);
    fclose(pidsMaxFile);

    waitpid(childPid, NULL, 0);

    return 0;
}
