#define _GNU_SOURCE
#include <sys/mount.h>
#include <string.h>
#include "../common/log.h"
#include "../common/utils.h"
#include "container.h"

int setup_overlay_rootfs(Container *c) {
    // Simplified bind mount - full overlayfs in v2
    if (mount(c->rootfs, "/", NULL, MS_BIND | MS_REC, NULL) != 0) {
        log_error("rootfs bind mount failed");
        return -1;
    }
    return 0;
}
