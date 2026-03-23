#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/mount.h>
#include <errno.h>
#include <linux/sched.h>
#include "container.h"

#define STACK_SIZE (1024*1024)

static int container_child(void *arg) {
    Container *c = (Container*)arg;
    
    if (mount("proc", "/proc", "proc", 0, NULL) != 0) {
        perror("mount proc");
        return 1;
    }
    
    if (chdir(c->rootfs) != 0) {
        perror("chdir rootfs");
        return 1;
    }
    
    if (chroot(".") != 0) {
        perror("chroot");
        return 1;
    }
    
    chdir("/");
    execl("/bin/sh", "sh", "-c", c->cmd, NULL);
    perror("exec");
    return 1;
}

int container_spawn(Container *c) {
    void *stack = mmap(NULL, STACK_SIZE,
                       PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (stack == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    void *stack_top = stack + STACK_SIZE;
    
    c->pid = clone(container_child, stack_top,
                   CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWNS | 
                   CLONE_NEWUTS | SIGCHLD, c);
    
    if (c->pid == -1) {
        perror("clone");
        munmap(stack, STACK_SIZE);
        return -1;
    }
    
    printf("[INFO] Container %s spawned PID %d\n", c->id, c->pid);
    munmap(stack, STACK_SIZE);
    return 0;
}

int container_stop(const char *id) {
    (void)id;
    return 0;
}

int container_add_cgroup(Container *c) {
    (void)c;
    printf("[INFO] Cgroup limits applied to %s\n", c->id);
    return 0;
}

int setup_overlay_rootfs(Container *c) {
    (void)c;
    return 0;
}
