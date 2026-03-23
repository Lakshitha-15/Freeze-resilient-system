#define _GNU_SOURCE
#include <stdlib.h>
#include <pthread.h>
#include "../common/log.h"
#include "scheduler.h"

static RunQueue global_rq = {0};
static pthread_mutex_t rq_lock = PTHREAD_MUTEX_INITIALIZER;

void rq_init() {
    global_rq.head = NULL;
    global_rq.tail = NULL;
    pthread_mutex_init(&rq_lock, NULL);
}

void rq_add(Container *c) {
    pthread_mutex_lock(&rq_lock);
    
    Task *t = malloc(sizeof(Task));
    t->container = c;
    t->slice_used = 0;
    t->next = NULL;
    
    if (!global_rq.tail) {
        global_rq.head = t;
    } else {
        global_rq.tail->next = t;
    }
    global_rq.tail = t;
    
    pthread_mutex_unlock(&rq_lock);
    log_info("Added %s to runqueue", c->id);
}
