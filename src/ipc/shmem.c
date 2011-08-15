#include "ipc/ipc.h"

#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "util.h"
#include "utils/sem.h"
#include <errno.h>
#include <stdlib.h>

#include <pthread.h>

#define ENTRIES_PER_QUEUE 8
#define ENTRIES_PER_SLOT (2 * ENTRIES_PER_QUEUE)

#define ENTRY_SIZE (IPC_MAX_PACKET_LEN + sizeof(size_t))

#define SHMEM_SIZE (ENTRY_SIZE * ENTRIES_PER_SLOT+ sizeof(struct ipc_t)) * IPC_MAX_CONNS

static int sharedFd;
static int assignedSlots;

static char* shmem;
static char* slots;
static char shmemName[255];

static sem_t slotLock;

static int availableReads;
static sem_t selectLock;
static sem_t selectWait;

struct Queue {
    int entryMap[ENTRIES_PER_QUEUE];
    sem_t readWait;
};

struct ipc_t {
    int slot;
    pid_t creator;
    pid_t second;
    sem_t lock;
    struct Queue firstQueue;
    struct Queue secondQueue;
};

enum Mode {
    ModeWrite,
    ModeRead
};

static char* getQueueSlot(ipc_t conn, enum Mode mode);

static ipc_t getHead(int slot);

static struct Queue* getQueue(ipc_t conn, enum Mode mode);

static size_t* getQueueEntry(ipc_t conn, int entry, enum Mode mode);

static void addMessage(void);

static void readMessage(void);

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
void fuckItUpYo(ipc_t conn) {
    struct Queue* queue;
    size_t* entry;
    pthread_mutex_lock(&lock);
    mprintf("First queue:\n");
    queue = &conn->firstQueue;
    for (int i = 0; i < ENTRIES_PER_QUEUE; i++) {
        mprintf("\t%d: %d\n", i, queue->entryMap[i]);
    }
    mprintf("On slots...\n");
    entry = (size_t*)(slots + conn->slot * ENTRIES_PER_SLOT * ENTRY_SIZE);
    for (int i = 0; i < ENTRIES_PER_QUEUE; i++) {
        mprintf("\t(%u) %d: %c\n", entry, i, *((char*)(entry + 1)));
        if (*entry) {
            mprintf("\t%d: %u bytes - %s\n", i, *entry, (char*)(entry + 1));
        }
        entry = (size_t*) (((char*) entry) + ENTRY_SIZE);
    }

    mprintf("Second queue:\n");
    queue = &conn->secondQueue;
    for (int i = 0; i < ENTRIES_PER_QUEUE; i++) {
        mprintf("\t%d: %d\n", i, queue->entryMap[i]);
    }
    mprintf("On slots...\n");
    for (int i = 0; i < ENTRIES_PER_QUEUE; i++) {
        if (*entry) {
            mprintf("\t%d: %u bytes - %s\n", i, *entry, (char*)(entry + 1));
        }
        entry += ENTRY_SIZE;
    }

    pthread_mutex_unlock(&lock);
}

int ipc_init(void) {
    sprintf(shmemName, "/arqvenger%d", getpid());

    sharedFd = shm_open(shmemName, O_CREAT | O_RDWR | 0666);
    if (sharedFd == -1) {
        return -1;
    }

    if (ftruncate(sharedFd, SHMEM_SIZE) == -1) {
        return -1;
    }

    shmem = mmap(NULL, SHMEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sharedFd, 0);
    if (shmem == (void*) -1) {
        shm_unlink(shmemName);
        return -1;
    }

    slots = shmem + sizeof(struct ipc_t) * IPC_MAX_CONNS;

    slotLock = ipc_sem_create(1);
    selectLock = ipc_sem_create(1);
    selectWait = ipc_sem_create(0);
    availableReads = 0;
    assignedSlots = 0;

    return 0;
}

void ipc_end(void) {
    ipc_sem_destroy(selectWait);
    ipc_sem_destroy(slotLock);
    munmap(shmem, SHMEM_SIZE);
    shm_unlink(shmemName);
}

ipc_t ipc_create(void) {

    ipc_t ipc;

    ipc_sem_wait(slotLock);
    if (assignedSlots == IPC_MAX_CONNS) {
        return NULL;
    }

    ipc = getHead(assignedSlots);
    ipc->slot = assignedSlots++;
    ipc_sem_post(slotLock);

    ipc->lock = ipc_sem_create(1);
    ipc->firstQueue.readWait = ipc_sem_create(0);
    ipc->secondQueue.readWait = ipc_sem_create(0);

    for (int i = 0; i < ENTRIES_PER_QUEUE; i++) {
        ipc->firstQueue.entryMap[i] = -1;
        ipc->secondQueue.entryMap[i] = -1;
    }

    ipc->creator = getpid();

    return ipc;
}

ipc_t ipc_establish(ipc_t conn, pid_t cpid) {

    if (cpid == 0) {
        conn->second = getpid();
    }

    for (int i = 0; i < ENTRIES_PER_QUEUE; i++) {
        *((size_t*) getQueueEntry(conn, i, ModeWrite)) = 0;
    }

    return conn;
}

void ipc_close(ipc_t conn) {
    ipc_sem_destroy(conn->lock);
    ipc_sem_destroy(conn->firstQueue.readWait);
    ipc_sem_destroy(conn->secondQueue.readWait);
    // We actually leak this slot, we dont plan on ever reusing it
}

int ipc_read(ipc_t conn, void* buff, size_t len) {
    int foundSlot = -1, res;
    size_t* entry;
    size_t size;
    struct Queue* queue;

    ipc_sem_wait(conn->lock);
    queue = getQueue(conn, ModeRead);
    while (foundSlot == -1) {
        foundSlot = queue->entryMap[0];
        if (foundSlot == -1) {
            ipc_sem_post(conn->lock);
            ipc_sem_wait(queue->readWait);
            ipc_sem_wait(conn->lock);
        }
    }

    readMessage();

    entry = getQueueEntry(conn, foundSlot, ModeRead);
    size = *entry;

    if (len > size) {
        len = size;
    }
    memcpy(buff, (void*)(entry + 1), len);
    *entry = 0;

    for (int i = 1; i < ENTRIES_PER_QUEUE; i++) {
        queue->entryMap[i - 1] = queue->entryMap[i];
    }
    queue->entryMap[ENTRIES_PER_QUEUE - 1] = -1;


    ipc_sem_post(conn->lock);
    return len;
}

int ipc_write(ipc_t conn, const void* buff, size_t len) {
    int slot, mapSlot, res;
    size_t* entry;
    struct Queue* queue;

    res = ipc_sem_wait(conn->lock);

    queue = getQueue(conn, ModeWrite);

    for (mapSlot = 0; mapSlot < ENTRIES_PER_QUEUE; mapSlot++) {
        if (queue->entryMap[mapSlot] == -1) {
            break;
        }
    }

    //FIXME: Make me block till we can write
    if (mapSlot == ENTRIES_PER_QUEUE) {
        ipc_sem_post(conn->lock);
        return -1;
    }

    for (slot = 0; slot < ENTRIES_PER_QUEUE; slot++) {
        if (*(entry = getQueueEntry(conn, slot, ModeWrite)) == 0) {
            break;
        }
    }

    if (len > IPC_MAX_PACKET_LEN) {
        len = IPC_MAX_PACKET_LEN;
    }

    queue->entryMap[mapSlot] = slot;
    *entry = len;
    memcpy((void*)(entry + 1), buff, len);

    ipc_sem_post(queue->readWait);
    addMessage();
    ipc_sem_post(conn->lock);

    return len;
}

char* getQueueSlot(ipc_t conn, enum Mode mode) {
    size_t slotOffset = 0;
    if (conn->creator == getpid()) {
        if (mode == ModeRead) {
            slotOffset = ENTRIES_PER_QUEUE * ENTRY_SIZE;
        }
    } else {
        if (mode == ModeWrite) {
            slotOffset = ENTRIES_PER_QUEUE * ENTRY_SIZE;
        }
    }

    return &slots[ENTRY_SIZE * ENTRIES_PER_SLOT * conn->slot + slotOffset];
}

ipc_t getHead(int slot) {
    return (ipc_t) &shmem[sizeof(struct ipc_t) * slot];
}

size_t* getQueueEntry(ipc_t conn, int entry, enum Mode mode) {
    return (size_t*)(getQueueSlot(conn, mode) + entry * ENTRY_SIZE);
}

struct Queue* getQueue(ipc_t conn, enum Mode mode) {

    if (mode == ModeRead) {
        if (conn->creator == getpid()) {
            return &conn->secondQueue;
        } else {
            return &conn->firstQueue;
        }
    } else {
        if (conn->creator == getpid()) {
            return &conn->firstQueue;
        } else {
            return &conn->secondQueue;
        }
    }
}

ipc_t ipc_select(void) {
    struct Queue* queue;
    ipc_t conn;
    pid_t myPid = getpid();
    ipc_sem_wait(selectLock);
    while (availableReads == 0) {
        ipc_sem_post(selectLock);
        ipc_sem_wait(selectWait);
        ipc_sem_wait(selectLock);
    }

    ipc_sem_wait(slotLock);
    for (int i = 0; i < assignedSlots; i++) {
        conn = getHead(i);
        if (conn && (conn->creator == myPid || conn->second == myPid)) {
            queue = getQueue(conn, ModeRead);
            if (queue->entryMap[0] != -1) {
                break;
            }
        }
        conn = NULL;
    }
    ipc_sem_post(slotLock);

    ipc_sem_post(selectLock);
    return conn;
}

void addMessage(void) {
    ipc_sem_wait(selectLock);
    availableReads++;
    ipc_sem_post(selectWait);
    ipc_sem_post(selectLock);
}

void readMessage(void) {
    ipc_sem_wait(selectLock);
    availableReads--;
    if (availableReads < 0) {
        mprintf("More messages were read than were written, that cant happen. Abort!\n");
        abort();
    }
    ipc_sem_post(selectLock);
}

