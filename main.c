#include <stdio.h>
#include <string.h>
#include "runtime/container.h"

// functions
void supervisor_loop();

int main() {
    printf("Starting Freeze-Resilient System...\n");

    Container c;

    strcpy(c.id, "container1");
    c.cmd = "/bin/sh";        // placeholder
    c.rootfs = "./test_rootfs"; // adjust if needed
    c.cpu_quota = 20000;
    c.cpu_period = 100000;
    c.mem_limit = 50 * 1024 * 1024; // 50MB

    container_spawn(&c);

    supervisor_loop();

    return 0;
}
