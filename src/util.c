#include "util.h"

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <unistd.h>
#include <execinfo.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>

#include "utils/sem.h"

static semv_t printLock = -1;

#define lock { \
    perform_lock(); \
    pthread_cleanup_push(perform_unlock, NULL);

#define unlock\
    pthread_cleanup_pop(1); \
}

static void perform_lock(void);

static void perform_unlock(void* u);

void perform_lock(void) {
    ipc_sem_wait(printLock);
}

void perform_unlock(void* u) {
    ipc_sem_post(printLock);
}

int mprintf_init(void) {
    printLock = ipc_sem_create(1);
    return printLock != -1;
}

int mprintf(const char* format, ...) {
    va_list ap;
    int res;
#ifndef DEBUG
    return 0;
#endif

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
#ifndef VERBOSE
    return;
#endif

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

    lock
        va_start(ap, format);
        printf("(%d) {ERR} ", getpid());
        vprintf(format, ap);
        fflush(stdout);
        va_end(ap);
    unlock
}

void print_errno(const char* tag) {
    int res;
    lock
        char buff[512];
        res = strerror_r(errno, buff, 511);
        buff[511] = 0;
        printf("(%d) {ERR} %s: %s\n", getpid(), tag, buff);
        fflush(stdout);
    unlock
}

