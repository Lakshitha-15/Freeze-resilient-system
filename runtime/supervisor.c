#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sched.h>
#include <string.h>

#include "container.h"

#define STACK_SIZE (1024 * 1024)

int container_child(void *arg);

// RESTART CONTAINER
void restart_container(int index) {
    printf("[SUPERVISOR] Restarting container %s...\n", containers[index].id);

    if (containers[index].restart_count >= 3) {
        printf("[SUPERVISOR] Restart limit reached for %s\n", containers[index].id);
        return;
    }

    containers[index].restart_count++;

    char *stack = malloc(STACK_SIZE);
    char *stack_top = stack + STACK_SIZE;

    int pid = clone(container_child, stack_top,
                    CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWUTS | SIGCHLD,
                    &containers[index]);

    containers[index].pid = pid;
    containers[index].last_heartbeat = time(NULL);
    containers[index].active = 1;
}

// WATCHDOG CHECK
void check_containers() {
    time_t now = time(NULL);

    for (int i = 0; i < MAX_CONTAINERS; i++) {
        if (containers[i].active) {

            double diff = difftime(now, containers[i].last_heartbeat);

            if (diff > 5) {
                printf("[WATCHDOG] %s frozen. Killing PID %d...\n",
                       containers[i].id, containers[i].pid);

                kill(containers[i].pid, SIGKILL);

                containers[i].active = 0;

                restart_container(i);
            }
        }
    }
}

// supervisor loop

void supervisor_loop() {
    while (1) {
        check_containers();
        sleep(2);
    }
}
