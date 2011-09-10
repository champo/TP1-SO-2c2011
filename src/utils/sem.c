#include "utils/sem.h"

#include <sys/types.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>

#include "util.h"

static key_t key = 0x1337C0DE;
static pthread_mutex_t keyLock = PTHREAD_MUTEX_INITIALIZER;

#ifndef __APPLE__
union semun {
    int val;
    struct semi_ds* buf;
    unsigned short *array;
};
#endif

semv_t ipc_sem_create(int value) {
    union semun opt;

    pthread_mutex_lock(&keyLock);
    semv_t sem;
    // NIGGA MODE ON
    while ((sem = semget(key++, 1, IPC_CREAT | IPC_EXCL | 0666)) == -1 && errno == EEXIST);
    // NIGGA MODE OFF
    if (sem == -1) {
#ifdef DEBUG
        print_errno("Semaphore creation failed");
        print_trace();
#endif
        pthread_mutex_unlock(&keyLock);
        return -1;
    }
    pthread_mutex_unlock(&keyLock);

    opt.val = value;
    if (semctl(sem, 0, SETVAL, opt) == -1) {
#ifdef DEBUG
        print_errno("Couldnt set the sem value");
#ifdef VERBOSE
        print_trace();
#endif
#endif

        ipc_sem_destroy(sem);
        return -1;
    }

#ifdef VERBOSE
    mprintf("Creating sem with id %d\n", sem);
    print_trace();
#endif

    return sem;
}

int ipc_sem_wait(semv_t sem) {
    struct sembuf op;

    op.sem_num = 0;
    op.sem_flg = 0;
    op.sem_op = -1;

    int res;
    while ((res = semop(sem, &op, 1)) == -1 && errno == EINTR);
#ifdef DEBUG
    if (res == -1) {
        print_errno("ipc_sem_wait failed");
#ifdef VERBOSE
        print_trace();
#endif
    }
#endif
    return res;
}

int ipc_sem_post(semv_t sem) {
    struct sembuf op;

    op.sem_num = 0;
    op.sem_flg = 0;
    op.sem_op = 1;

    int res;
    while ((res = semop(sem, &op, 1)) == -1 && errno == EINTR);
#ifdef DEBUG
    if (res == -1) {
        print_errno("ipc_sem_post failed");
#ifdef VERBOSE
        print_trace();
#endif
    }
#endif
    return res;
}

int ipc_sem_value(semv_t sem) {
    int res = semctl(sem, 0, GETVAL);
    if (res == -1) {
        print_errno("failed reading the semaphore value");
#ifdef VERBOSE
        print_trace();
#endif
    }
    return res;
}

int ipc_sem_destroy(semv_t sem) {

    if (-1 == sem) {
        return 0;
    }

    return semctl(sem, 0, IPC_RMID);
}

