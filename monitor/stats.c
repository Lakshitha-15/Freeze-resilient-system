#define _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "../../common/log.h"

typedef struct {
    char id[32];
    long mem_usage;
    int cpu_usage;
} ContainerStats;

int read_container_stats(ContainerStats *stats, const char *cid) {
    char path[256];
    snprintf(path, sizeof(path), "/sys/fs/cgroup/%s/memory.current", cid);
    
    FILE *f = fopen(path, "r");
    if (f) {
        fscanf(f, "%ld", &stats->mem_usage);
        fclose(f);
    }
    return 0;
}
