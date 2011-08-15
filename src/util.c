#include "util.h"

#include <stdio.h>
#include <unistd.h>
#include <execinfo.h>
#include <stdlib.h>

#include "utils/sem.h"

static sem_t lock;

int mprintf_init(void) {
    lock = ipc_sem_create(1);
    return lock != -1;
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

void print_trace(void) {

    void *array[10];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);

    for (i = 0; i < size; i++) {
        printf ("%s\n", strings[i]);
    }

    free (strings);
}

