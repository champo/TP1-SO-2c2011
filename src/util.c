#include "util.h"

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t lock;

void init_mprintf(void) {
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&lock, &attr);

    pthread_mutexattr_destroy(&attr);
}

int mprintf(const char* format, ...) {
    va_list ap;
    int res;

    pthread_mutex_lock(&lock);
    va_start(ap, format);
    printf("(%d) ", getpid());
    res = vprintf(format, ap);
    fflush(stdout);
    va_end(ap);
    pthread_mutex_unlock(&lock);

    return res;
}

