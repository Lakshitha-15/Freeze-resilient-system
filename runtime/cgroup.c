#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "common/log.h"
#include "common/utils.h"
#include "container.h"

static int write_cgroup(const char *cid, const char *file, const char *val) {
    char path[256];
    snprintf(path, sizeof(path), "/sys/fs/cgroup/%s/%s", cid, file);
    
    FILE *f = fopen(path, "we");
    if (!f) {
        log_error("cgroup write %s failed: %s", path, strerror(errno));
        return -1;
    }
    fprintf(f, "%s\n", val);
    fflush(f);
    fclose(f);
    return 0;
}

int container_add_cgroup(Container *c) {
    char cgroup_path[256];
    snprintf(cgroup_path, sizeof(cgroup_path), "/sys/fs/cgroup/%s", c->id);
    
    // Create cgroup
    if (mkdir(cgroup_path, 0755) != 0 && errno != EEXIST) {
        log_error("cgroup mkdir failed: %s", strerror(errno));
        return -1;
    }
    
    // Enable controllers
    write_cgroup(c->id, "cgroup.subtree_control", "+cpu +memory");
    
    // CPU quota
    char cpu_max[64];
    snprintf(cpu_max, sizeof(cpu_max), "%d %d", c->cpu_quota, c->cpu_period);
    write_cgroup(c->id, "cpu.max", cpu_max);
    
    // Memory limit
    char mem_max[32];
    snprintf(mem_max, sizeof(mem_max), "%ld", c->mem_limit);
    write_cgroup(c->id, "memory.max", mem_max);
    
    // Add PID to cgroup
    char procs[32];
    snprintf(procs, sizeof(procs), "%d", c->pid);
    write_cgroup(c->id, "cgroup.procs", procs);
    
    log_info("Cgroup limits applied to %s (CPU: %d/%d, MEM: %ld)", 
             c->id, c->cpu_quota, c->cpu_period, c->mem_limit);
    return 0;
}
