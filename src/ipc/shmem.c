#include "ipc/ipc.h"

#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>

#define MAX_QUEUE_SIZE 10

#define ENTRY_SIZE (IPC_MAX_PACKET_LEN + 2*sizeof(size_t))

#define QUEUE_SIZE (ENTRY_SIZE * MAX_QUEUE_SIZE + sizeof(struct ipc_t)) * IPC_MAX_CONNS

int sharedFd;
int assignedSlots;

char* queue;
char* slots;
char shmemName[255];

struct ipc_t {
    int slot;
    sem_t lock;
    sem_t readWait;
};

char* getQueueSlot(int slot);

ipc_t getHead(int slot);

char* getQueueEntry(int slot, int entry);

int ipc_init(void) {
    sprintf(shmemName, "/arqvenger%d", getpid());

    sharedFd = shm_open(shmemName, O_RDWR, 0);
    if (sharedFd == -1) {
        return -1;
    }

    if (ftruncate(sharedFd, QUEUE_SIZE) == -1) {
        return -1;
    }

    queue = mmap(NULL, QUEUE_SIZE, PROT_READ | PROT_WRITE, 0, 0);
    if (queue == (void*) -1) {
        shm_unlink(shmemName);
        return -1;
    }

    assignedSlots = 0;

    return 0;
}

void ipc_end(void) {
    munmap(queue, QUEUE_SIZE);
    shm_unlink(shmemName);
}

ipc_t ipc_create(void) {

    ipc_t ipc;
    int* slot;

    if (assignedSlots == IPC_MAX_CONNS) {
        return NULL;
    }

    ipc = getHead(assignedSlots);
    ipc->slot = assignedSlots++;
    sem_init(&ipc->lock, 1, 1);
    sem_init(&ipc->readWait, 1, 0);

    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        *((int*) getQueueSlot(ipc->slot, i)) = 0;
    }

    return ipc;
}

ipc_t ipc_establish(ipc_t conn, pid_t cpid) {
    return conn;
}

void ipc_close(ipc_t conn) {
    sem_destroy(&conn->lock);
    sem_destroy(&conn->readWait);
}

int ipc_read(ipc_t conn, void* buff, size_t len) {
    int foundSlot = 0;
    int* entry;
    int size;

    sem_wait(&conn->lock);
    while (!foundSlot) {
        entry = getQueueEntry(conn->slot, 0);
        foundSlot = entry[0] != 0;
        if (!foundSlot) {
            sem_post(&conn->lock);
            sem_wait(&conn->readWait);
            sem_wait(&conn->lock);
        }
    }

    size = entry[0];
    memcpy(buff, (void*)(entry + 1), size);

    sem_post(&conn->lock);
    return size;
}

int ipc_write(ipc_t conn, void* buff, size_t len) {
    int slot;
    int* entry;

    sem_wait(&conn->lock);
    for (slot = 0; slot < MAX_QUEUE_SIZE; slot++) {
        entry = getQueueEntry(conn->slot, slot);
        if (entry[0] == 0) {
            break;
        }
    }

    if (slot == MAX_QUEUE_SIZE) {
        sem_post(&conn->lock);
        return -1;
    }

    entry[0] = len;
    memcpy((void*)(entry + 1), buff, len);

    sem_post(&conn->readWait);
    sem_post(&conn->lock);

    return len;
}

char* getQueueSlot(int slot) {
    return (int*) &slots[ENTRY_SIZE * MAX_QUEUE_SIZE * slot];
}

ipc_t getHead(int slot) {
    return (ipc_t) &queue[sizeof(struct ipc_t) * assignedSlots];
}

char* getQueueEntry(int slot, int entry) {
    return getQueueSlot(slot) + entry * ENTRY_SIZE;
}

