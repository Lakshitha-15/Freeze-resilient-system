#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "container.h"

void setup_memory_cgroup(Container *c);
void add_to_cgroup(Container *c);

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
//          malloc(1024 * 1024); //1MB leak every loop

        printf("[Container %s] Running... PID=%d\n", c->id, pid);
        update_heartbeat(pid);
        sleep(1);
    }

    return 0;
}

// SET MEMORY LIMIT USING CGROUP
void setup_memory_cgroup(Container *c) {
    char path[512];

    // create cgroup directory
    snprintf(path, sizeof(path),
             "/sys/fs/cgroup/%s", c->id);
    mkdir(path, 0755);

    // set memory limit
    char mem_path[512];
    snprintf(mem_path, sizeof(mem_path),
             "%s/memory.max", path);

    int fd = open(mem_path, O_WRONLY);
    if (fd < 0) {
        perror("open memory.max");
        return;
    }

    char limit[64];
    sprintf(limit, "%ld", c->mem_limit);

    write(fd, limit, strlen(limit));
    close(fd);

    printf("[CGROUP] Memory limit set: %ld bytes\n", c->mem_limit);
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
    //SET MEMORY LIMIT
    setup_memory_cgroup(c);
    //ADD PROCESS TO CGROUP
    add_to_cgroup(c);
    register_container(c);

/*
    c->pid = pid;
    printf("[RUNTIME] Container %s started with PID %d\n", c->id, pid);
    register_container(c);
*/
    return pid;
}

void add_to_cgroup(Container *c) {
    char path[512];

    snprintf(path, sizeof(path),
             "/sys/fs/cgroup/%s/cgroup.procs", c->id);

    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("open cgroup.procs");
        return;
    }

    char pid_str[32];
    sprintf(pid_str, "%d", c->pid);

    write(fd, pid_str, strlen(pid_str));
    close(fd);

    printf("[CGROUP] Added PID %d to cgroup\n", c->pid);
}
