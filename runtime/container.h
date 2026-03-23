#ifndef CONTAINER_H
#define CONTAINER_H
#include <sys/types.h>

typedef struct {
    pid_t pid;
    char id[32];
    char *cmd;
    char *rootfs;
    int cpu_quota;
    int cpu_period;
    long mem_limit;
} Container;

int container_spawn(Container *c);
int container_stop(const char *id);
int container_add_cgroup(Container *c);
int setup_overlay_rootfs(Container *c);

#endif
