#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../../common/log.h"
#include "../../common/utils.h"

void *heartbeat_sender(void *arg) {
    Container *c = (Container*)arg;
    char path[256];
    snprintf(path, sizeof(path), "/tmp/heartbeat_%s", c->id);
    
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return NULL;
    
    while (1) {
        write(fd, "alive", 5);
        fsync(fd);
        sleep(1);
    }
    close(fd);
    return NULL;
}
