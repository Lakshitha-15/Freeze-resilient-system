#ifndef STATS_H
#define STATS_H

typedef struct {
    char id[32];
    long mem_usage;
    int cpu_usage;
} ContainerStats;

int read_container_stats(ContainerStats *stats, const char *cid);

#endif
