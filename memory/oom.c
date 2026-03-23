#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "../../common/log.h"
#include "oom.h"

static int read_cgroup_mem(const char *cid) {
    char path[256];
    snprintf(path, sizeof(path), "/sys/fs/cgroup/%s/memory.current", cid);
    
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    
    long usage;
    fscanf(f, "%ld", &usage);
    fclose(f);
    return usage;
}

static void *oom_poller(void *arg) {
    Container *c = (Container*)arg;
    
    while (1) {
        long usage = read_cgroup_mem(c->id);
        if (usage > 0 && usage > c->mem_limit * 0.9) {
            log_warn("OOM: %s usage %ld/%ld (90%%)", c->id, usage, c->mem_limit);
            kill(c->pid, SIGTERM);
        }
        sleep(1);
    }
    return NULL;
}

int oom_watch_container(Container *c) {
    pthread_t thread;
    pthread_create(&thread, NULL, oom_poller, c);
    pthread_detach(thread);
    log_info("OOM watcher started for %s", c->id);
    return 0;
}
