#ifndef CONTAINER_H
#define CONTAINER_H

#include <sys/types.h>
#include <time.h>

#define MAX_CONTAINERS 10

typedef struct {
    pid_t pid;
    char id[32];
    char *cmd;
    char *rootfs;
    int cpu_quota;
    int cpu_period;
    long mem_limit;

    time_t last_heartbeat;
    int restart_count;
    int active;

} Container;

extern Container containers[MAX_CONTAINERS];

int container_spawn(Container *c);
int container_stop(const char *id);
int container_add_cgroup(Container *c);
int setup_overlay_rootfs(Container *c);

void register_container(Container *c);
void update_heartbeat(pid_t pid);

#endif
