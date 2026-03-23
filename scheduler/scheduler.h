#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "../runtime/container.h"  // Container lives here now

typedef struct Task {
    Container *container;
    int slice_used;
    struct Task *next;
} Task;

typedef struct {
    Task *head;
    Task *tail;
} RunQueue;

void scheduler_init();
void scheduler_tick();
void rq_add(Container *c);
void rq_remove(pid_t pid);

#endif
