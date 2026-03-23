#define _GNU_SOURCE
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include "../../common/log.h"
#include "watchdog.h"

static void *watchdog_thread(void *arg) {
    while (1) {
        DIR *dir = opendir("/tmp");
        if (!dir) continue;
        
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strncmp(entry->d_name, "heartbeat_", 10) == 0) {
                char path[256];
                snprintf(path, sizeof(path), "/tmp/%s", entry->d_name);
                
                struct stat st;
                if (stat(path, &st) == 0) {
                    if (time(NULL) - st.st_mtime > 5) {
                        char cid[32];
                        sscanf(entry->d_name, "heartbeat_%31s", cid);
                        log_error("Watchdog: %s frozen - killing", cid);
                        // TODO: kill container
                    }
                }
            }
        }
        closedir(dir);
        sleep(2);
    }
    return NULL;
}

int watchdog_start() {
    pthread_t thread;
    pthread_create(&thread, NULL, watchdog_thread, NULL);
    pthread_detach(thread);
    log_info("Watchdog started");
    return 0;
}
