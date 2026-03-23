#define _GNU_SOURCE
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include "../common/log.h"
#include "scheduler.h"

static RunQueue global_rq;

static void sigalrm_handler(int sig) {
    scheduler_tick();
}

void scheduler_init() {
    rq_init();
    
    struct sigaction sa = {0};
    sa.sa_handler = sigalrm_handler;
    sigaction(SIGALRM, &sa, NULL);
    
    // 10ms timer
    struct itimerval timer = {0};
    timer.it_interval.tv_usec = 10000;  // 10ms
    timer.it_value.tv_usec = 10000;
    setitimer(ITIMER_REAL, &timer, NULL);
    
    log_info("Scheduler initialized (10ms ticks)");
}

void scheduler_tick() {
    // Simple round-robin: pause if slice exceeded
    Task *curr = global_rq.head;
    if (curr && curr->container) {
        curr->slice_used++;
        
        if (curr->slice_used >= 10) {  // 100ms slice
            kill(curr->container->pid, SIGSTOP);
            log_info("Paused %s (slice exhausted)", curr->container->id);
            
            // Resume after short delay (next tick)
            usleep(1000);
            kill(curr->container->pid, SIGCONT);
            curr->slice_used = 0;
        }
    }
}
