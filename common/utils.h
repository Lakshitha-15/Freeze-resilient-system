#ifndef UTILS_H
#define UTILS_H
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

int str_to_long(const char *s, long *result);
long get_us_time(void);

#endif
