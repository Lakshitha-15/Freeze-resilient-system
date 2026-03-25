#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "container.h"

#define STACK_SIZE (1024 * 1024)

Container containers[MAX_CONTAINERS] = {0};

void register_container(Container *c) {
    for (int i = 0; i < MAX_CONTAINERS; i++) {
        if (!containers[i].active) {
            containers[i] = *c;
            containers[i].last_heartbeat = time(NULL);
            containers[i].restart_count = 0;
            containers[i].active = 1;
            break;
        }
    }
}

void update_heartbeat(pid_t pid) {
    for (int i = 0; i < MAX_CONTAINERS; i++) {
        if (containers[i].pid == pid && containers[i].active) {
            containers[i].last_heartbeat = time(NULL);
            break;
        }
    }
}

int container_child(void *arg) {
    Container *c = (Container *)arg;
    pid_t pid = getpid();

    while (1) {
        printf("[Container %s] Running... PID=%d\n", c->id, pid);

        update_heartbeat(pid);

        sleep(1);
    }

    return 0;
}

int container_spawn(Container *c) {
    char *stack = malloc(STACK_SIZE);
    if (!stack) {
        perror("malloc");
        return -1;
    }

    char *stack_top = stack + STACK_SIZE;

    int pid = clone(container_child, stack_top,
                    CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWUTS | SIGCHLD,
                    c);

    if (pid == -1) {
        perror("clone");
        return -1;
    }

    c->pid = pid;

    printf("[RUNTIME] Container %s started with PID %d\n", c->id, pid);

    register_container(c);

    return pid;
}
