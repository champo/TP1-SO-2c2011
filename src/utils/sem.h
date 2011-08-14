#ifndef __UTILS_SEM__
#define __UTILS_SEM__

typedef int sem_t;

sem_t ipc_sem_create(int value);

int ipc_sem_wait(sem_t sem);

int ipc_sem_post(sem_t sem);

int ipc_sem_value(sem_t sem);

int ipc_sem_destroy(sem_t sem);

#endif
