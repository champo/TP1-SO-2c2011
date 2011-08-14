#include "ipc/ipc.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>

#include "util.h"

#define SOCKET_FILE "sim.sock"

struct ipc_t {
    int fd;
    pthread_mutex_t mutex;
};

static void connection_listener_cleanup(void* arg);
static void* wait_for_connection(int fd);

static pthread_t listener = NULL;
static struct ipc_t* lastCreated = NULL;
static pthread_mutex_t lastCreatedMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t lastCreatedCond = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t connLock = PTHREAD_MUTEX_INITIALIZER;
static ipc_t openConnections[IPC_MAX_CONNS];
static size_t numConnections;

int ipc_init(void) {
    int fd;
    struct sockaddr_un addr;

    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd == -1) {
        return -1;
    }

    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, SOCKET_FILE);

    if (bind(fd, (struct sockaddr*) &addr, sizeof(struct sockaddr)) != 0) {
        return -1;
    }

    numConnections = 0;
    memset(openConnections, 0, sizeof(ipc_t) * IPC_MAX_CONNS);

    return pthread_create(&listener, NULL, (void*(*)(void*))wait_for_connection, (void*) fd);
}

void ipc_end(void) {
    if (listener) {
        pthread_cancel(listener);
        unlink(SOCKET_FILE);
    }
    pthread_mutex_destroy(&lastCreatedMutex);
    pthread_cond_destroy(&lastCreatedCond);
}

ipc_t ipc_create(void) {
    struct ipc_t* ipc = malloc(sizeof(struct ipc_t));
    ipc->fd = 0;
    pthread_mutex_init(&ipc->mutex, NULL);

    pthread_mutex_lock(&lastCreatedMutex);
    lastCreated = ipc;
    pthread_mutex_unlock(&lastCreatedMutex);

    return ipc;
}

ipc_t ipc_establish(ipc_t conn, pid_t cpid) {
    struct ipc_t* sock = (struct ipc_t*) conn;
    if (cpid == 0) {
        pthread_cancel(listener);
        listener = NULL;

        // This is a child process, so we connect
        struct sockaddr_un addr;

        sock->fd = socket(PF_LOCAL, SOCK_STREAM, 0);
        if (sock->fd == -1) {
            return NULL;
        }

        addr.sun_family = AF_LOCAL;
        strcpy(addr.sun_path, SOCKET_FILE);

        if (connect(sock->fd, (struct sockaddr*) &addr, sizeof(struct sockaddr)) != 0) {
            return NULL;
        }
    } else {

        pthread_mutex_lock(&lastCreatedMutex);
        if (lastCreated->fd == 0) {
            //If we don't have a fd already, we wait for one
            pthread_cond_wait(&lastCreatedCond, &lastCreatedMutex);
        }

        lastCreated = NULL;

        // This means the accept failed
        if (conn->fd == 0) {
            return NULL;
        }

        pthread_mutex_unlock(&lastCreatedMutex);
        // We don't need to return shiet. conn already has the values.
    }
    pthread_mutex_lock(&connLock);
    openConnections[numConnections++] = conn;
    pthread_mutex_unlock(&connLock);

    return conn;
}

int ipc_read(ipc_t conn, void* buff, size_t len) {
    size_t size = 0;

    pthread_mutex_lock(&conn->mutex);
    read(conn->fd, &size, sizeof(size_t));
    read(conn->fd, buff, (size > len) ? len : size);
    pthread_mutex_unlock(&conn->mutex);

    return size;
}

int ipc_write(ipc_t conn, const void* buff, size_t len) {
    size_t size;

    pthread_mutex_lock(&conn->mutex);
    write(conn->fd, &len, sizeof(size_t));
    size = write(conn->fd, buff, len);
    pthread_mutex_unlock(&conn->mutex);

    return size;
}

void ipc_close(ipc_t conn) {
    struct ipc_t* sock = conn;
    close(sock->fd);
    pthread_mutex_destroy(&sock->mutex);
    free(sock);
}

void* wait_for_connection(int fd) {
    int conn;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_cleanup_push(connection_listener_cleanup, (void*) fd);
    while (1) {
        if (listen(fd, 1) == 0) {
            conn = accept(fd, NULL, NULL);
            pthread_mutex_lock(&lastCreatedMutex);
            if (lastCreated != NULL) {
                lastCreated->fd = conn;
            } else {
                close(conn);
            }
            pthread_cond_signal(&lastCreatedCond);
            pthread_mutex_unlock(&lastCreatedMutex);
        }
    }
    pthread_cleanup_pop(0);
    return NULL;
}

void connection_listener_cleanup(void* arg) {
    pthread_mutex_unlock(&lastCreatedMutex);
    close((int) arg);
}

ipc_t ipc_select(void) {
    ipc_t conn;
    fd_set fds;
    int maxFd = 0;

    pthread_mutex_lock(&connLock);

    for (size_t i = 0; i < numConnections; i++) {
        conn = openConnections[i];
        if (conn != NULL) {
            FD_SET(conn->fd, &fds);
            if (maxFd < conn->fd) {
                maxFd = conn->fd;
            }
        }
    }

    if (select(maxFd + 1, &fds, NULL, NULL, NULL) == -1) {
        conn = NULL;
    } else {
        for (size_t i = 0; i < numConnections; i++) {
            conn = openConnections[i];
            if (conn && FD_ISSET(conn->fd, &fds)) {
                break;
            } else {
                conn = NULL;
            }
        }
    }

    pthread_mutex_unlock(&connLock);
    return conn;
}

