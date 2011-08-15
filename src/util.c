#include "util.h"

#include <stdio.h>
#include <unistd.h>

#include "utils/sem.h"

static sem_t lock;

void mprintf_init(void) {
    lock = ipc_sem_create(1);
}

int mprintf(const char* format, ...) {
    va_list ap;
    int res;

    ipc_sem_wait(lock);
    va_start(ap, format);
    printf("(%d) ", getpid());
    res = vprintf(format, ap);
    fflush(stdout);
    va_end(ap);
    ipc_sem_post(lock);

    return res;
}

void mprintf_end(void) {
    ipc_sem_destroy(lock);
    lock = 0;
}

