#include "util.h"

#include <stdio.h>
#include <unistd.h>
#include <execinfo.h>
#include <stdlib.h>
#include <errno.h>

#include "utils/sem.h"

static semv_t printLock = -1;

#define lock { \
    int hasSem = printLock != -1; \
    if (hasSem) { \
        ipc_sem_wait(printLock); \
    }

#define unlock \
        if (hasSem) { \
            ipc_sem_post(printLock); \
        } \
    }

int mprintf_init(void) {
    printLock = ipc_sem_create(1);
    return printLock != -1;
}

int mprintf(const char* format, ...) {
    va_list ap;
    int res;

    lock
        va_start(ap, format);
        printf("(%d) ", getpid());
        res = vprintf(format, ap);
        fflush(stdout);
        va_end(ap);
    unlock

    return res;
}

void mprintf_end(void) {
    ipc_sem_destroy(printLock);
    printLock = 0;
}

void print_trace(void) {

    void *array[10];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);

    lock
        for (i = 0; i < size; i++) {
            printf("(%d) %s\n", getpid(), strings[i]);
        }
    unlock

    free(strings);
}

void print_error(const char* format, ...) {
    va_list ap;
    int res;

    lock
        va_start(ap, format);
        printf("(%d) {ERR} ", getpid());
        res = vprintf(format, ap);
        fflush(stdout);
        va_end(ap);
    unlock

    return res;
}

void print_errno(const char* tag) {
    lock
        char buff[512];
        strerror_r(errno, buff, 512);
        buff[512] = 0;
        printf("(%d) {ERR} %s: %s\n", getpid(), tag, buff);
        fflush(stdout);
    unlock
}

