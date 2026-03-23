#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "runtime/container.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <id> <rootfs> <cmd>\n", argv[0]);
        printf("Example: %s cont1 test_rootfs \"while true; sleep 1\"\n", argv[0]);
        return 1;
    }
    
    Container c = {0};
    strncpy(c.id, argv[1], sizeof(c.id)-1);
    c.rootfs = argv[2];
    c.cmd = argv[3];
    
    printf("[INFO] Starting container %s\n", c.id);
    
    if (container_spawn(&c) != 0) {
        printf("[ERROR] Failed to spawn container\n");
        return 1;
    }
    
    printf("[INFO] Container %s running (PID %d) - Waiting...\n", c.id, c.pid);
    sleep(5);
    
    return 0;
}
