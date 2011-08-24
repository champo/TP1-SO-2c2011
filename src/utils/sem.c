#include "utils/sem.h"

#include <sys/types.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <pthread.h>
#include <errno.h>

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

sem_t ipc_sem_create(int value) {
    union semun opt;

    pthread_mutex_lock(&keyLock);
    sem_t sem = semget(key++, 1, O_CREAT | 0666);
    pthread_mutex_unlock(&keyLock);
    if (sem == -1) {
#ifdef DEBUG
        perror("Semaphore creation failed");
        print_trace();
#endif
        return -1;
    }

    opt.val = value;
    semctl(sem, 0, SETVAL, opt);

#ifdef VERBOSE
    mprintf("Creating sem with id %d\n", sem);
    print_trace();
#endif

    return sem;
}

int ipc_sem_wait(sem_t sem) {
    struct sembuf op;

    op.sem_num = 0;
    op.sem_flg = 0;
    op.sem_op = -1;

    return semop(sem, &op, 1);
}

int ipc_sem_post(sem_t sem) {
    struct sembuf op;

    op.sem_num = 0;
    op.sem_flg = 0;
    op.sem_op = 1;

    return semop(sem, &op, 1);
}

int ipc_sem_value(sem_t sem) {
    return semctl(sem, 0, GETVAL);
}

int ipc_sem_destroy(sem_t sem) {

    if (-1 == sem) {
        return 0;
    }

    return semctl(sem, 0, IPC_RMID);
}

