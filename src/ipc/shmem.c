#include "ipc/ipc.h"

#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>

#include "util.h"
#include "utils/sem.h"
#include <errno.h>
#include <stdlib.h>

#include <pthread.h>

#define ENTRIES_PER_QUEUE 16

#define ENTRY_SIZE sizeof(struct Entry)
#define SHMEM_SIZE sizeof(struct Queue)

#define echo(msg) mprintf("[ipc] "msg"\n");

struct Entry {
    size_t len;
    char content[IPC_MAX_PACKET_LEN];
};

struct Queue {
    int index[ENTRIES_PER_QUEUE];
    semv_t readWait;
    semv_t writeSem;
    struct Entry slots[ENTRIES_PER_QUEUE];
};

struct ipc_t {
    int fd;
    char name[512];
    sem_t* lock;
    struct Queue* queue;
};

static ipc_t ipc_create(const char* name, int owner);

static void ipc_destroy(ipc_t conn, int owner);

static struct ipc_t* me;

int ipc_init(void) {
    return 0;
}

ipc_t ipc_create(const char* name, int owner) {

    int isInited = 0;
    struct Queue* queue;

    ipc_t conn = malloc(sizeof(struct ipc_t));

    sprintf(conn->name, "/arqvenger_%s", name);

    if ((conn->lock = sem_open(conn->name, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) {
        conn->lock = sem_open(conn->name, 0);
        if (conn->lock == SEM_FAILED) {
            perror("Failed adquiring named lock");
            return NULL;
        }

        echo("The sem exists, lock..\n");
        sem_wait(conn->lock);
        isInited = 1;
        echo("The thing is inited, going for it..\n");
    } else {
        echo("The sem aint inited, doint it myslef...\n");
    }

    conn->fd = shm_open(conn->name, O_CREAT | O_RDWR, 0666);
    if (conn->fd == -1) {
        perror("shm_open failed");

        sem_close(conn->lock);
        if (owner) {
            sem_unlink(conn->name);
        }
        free(conn);

        return NULL;
    }

    if (!isInited) {
        if (ftruncate(conn->fd, SHMEM_SIZE) == -1) {
            perror("Truncate failed");

            sem_close(conn->lock);
            if (owner) {
                sem_unlink(conn->name);
                shm_unlink(conn->name);
            }
            free(conn);
            return NULL;
        }
    }

    conn->queue = mmap(NULL, SHMEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, conn->fd, 0);
    if (conn->queue == (void*) -1) {
        perror("mmap");

        sem_close(conn->lock);
        if (owner) {
            sem_unlink(conn->name);
            shm_unlink(conn->name);
        }

        free(conn);
        return NULL;
    }

    if (isInited) {
        sem_post(conn->lock);
        return conn;
    }

    queue = conn->queue;
    for (size_t i = 0; i < ENTRIES_PER_QUEUE; i++) {
        queue->index[i] = -1;
        queue->slots[i].len = 0;
    }

    queue->readWait = ipc_sem_create(0);
    queue->writeSem = ipc_sem_create(ENTRIES_PER_QUEUE);
    echo("Creating sems");
    if (queue->readWait == -1 || queue->writeSem == -1) {
        perror("failed creating sems");
        sem_close(conn->lock);
        ipc_sem_destroy(queue->readWait);
        ipc_sem_destroy(queue->writeSem);

        munmap(conn->queue, SHMEM_SIZE);
        if (owner) {
            sem_unlink(conn->name);
            shm_unlink(conn->name);
        }
        free(conn);
        return NULL;
    }

    sem_post(conn->lock);

    return conn;
}

void ipc_destroy(ipc_t conn, int owner) {

    if (conn == NULL) {
        return;
    }

    // It goes without saying that the order of these calls is important
    sem_close(conn->lock);

    if (owner) {
        ipc_sem_destroy(conn->queue->readWait);
        ipc_sem_destroy(conn->queue->writeSem);
    }

    munmap(conn->queue, SHMEM_SIZE);

    if (owner) {
        shm_unlink(conn->name);
        sem_unlink(conn->name);
    }

    free(conn);
}

int ipc_listen(const char* name) {
    me = ipc_create(name, 1);
    return me == NULL ? -1 : 0;
}

void ipc_end(void) {
    ipc_destroy(me, 1);
}

ipc_t ipc_establish(const char* name) {
    return ipc_create(name, 0);
}

void ipc_close(ipc_t conn) {
    ipc_destroy(conn, 0);
}

void assertSems(struct Queue* queue) {
    int read = ipc_sem_value(queue->readWait);
    if (read < 0 || read > ENTRIES_PER_QUEUE) {
        mprintf("[ipc] Read sem has invalid value %d\n", read);
    }
    int write = ipc_sem_value(queue->writeSem);
    if (write < 0 || write > ENTRIES_PER_QUEUE) {
        mprintf("[ipc] write sem has invalid value %d\n", write);
    }
}

int ipc_read(void* buff, size_t len) {

    int res;
    struct Queue* queue = me->queue;
    struct Entry* entry;

    mprintf("[ipc] (Pre read) Read: %d Write %d\n", ipc_sem_value(queue->readWait), ipc_sem_value(queue->writeSem));
    ipc_sem_wait(queue->readWait);
    sem_wait(me->lock);
    assertSems(queue);
    mprintf("[ipc] (Post read) Read: %d Write %d\n", ipc_sem_value(queue->readWait), ipc_sem_value(queue->writeSem));

    entry = &(queue->slots[queue->index[0]]);
    if (len > entry->len) {
        res = entry->len;
    } else {
        res = len;
    }
    memcpy(buff, entry->content, res);

    entry->len = 0;
    for (size_t i = 1; i < ENTRIES_PER_QUEUE; i++) {
        queue->index[i - 1] = queue->index[i];
    }

    // We always make sure to reset the last value, in case the queue was full
    queue->index[ENTRIES_PER_QUEUE - 1] = -1;

    echo("Posting for the write slot and leaving");
    sem_post(me->lock);
    ipc_sem_post(queue->writeSem);

    return res;
}

int ipc_write(ipc_t conn, const void* buff, size_t len) {
    size_t slot, i;
    struct Queue* queue = conn->queue;
    struct Entry* entry;

    mprintf("[ipc] (Pre Write) Read: %d Write %d\n", ipc_sem_value(queue->readWait), ipc_sem_value(queue->writeSem));
    ipc_sem_wait(me->queue->writeSem);
    sem_wait(me->lock);
    assertSems(queue);
    mprintf("[ipc] (Post Write) Read: %d Write %d\n", ipc_sem_value(queue->readWait), ipc_sem_value(queue->writeSem));

    // If we got this far, we know we have a place to write on!
    for (slot = 0; slot < ENTRIES_PER_QUEUE; slot++) {
        if (queue->slots[slot].len == 0) {
            break;
        }
    }

    for (i = 0; i < ENTRIES_PER_QUEUE; i++) {
        if (queue->index[i] == -1) {
            break;
        }
    }
    // I could sanity check those values, and I also could start working out.
    if (slot == ENTRIES_PER_QUEUE) {
        echo("We're humped");
    }
    if (i == ENTRIES_PER_QUEUE) {
        echo("WERE ALL GONNA DIE");
    }

    queue->index[i] = slot;
    entry = &queue->slots[slot];
    if (len > IPC_MAX_PACKET_LEN) {
        len = IPC_MAX_PACKET_LEN;
        echo("Doomed.");
    }
    entry->len = len;
    memcpy(entry->content, buff, len);

    echo("Posting read & bailing");
    sem_post(me->lock);
    ipc_sem_post(queue->readWait);

    return len;
}

