#ifndef __UTILS_SEM__
#define __UTILS_SEM__

typedef int semv_t;

semv_t ipc_sem_create(int value);

int ipc_sem_wait(semv_t sem);

int ipc_sem_post(semv_t sem);

int ipc_sem_value(semv_t sem);

int ipc_sem_destroy(semv_t sem);

#endif
