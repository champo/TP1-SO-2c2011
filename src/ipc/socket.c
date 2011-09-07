#include "ipc/ipc.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "util.h"

#ifdef __APPLE__
#define DOMAIN PF_LOCAL
#else
#define DOMAIN AF_UNIX
#endif

struct ipc_t {
    struct sockaddr_un addr;
    pthread_mutex_t mutex;
};

pthread_mutex_t readLock = PTHREAD_MUTEX_INITIALIZER;
int readSocket = -1;
int writeSocket = -1;
char path[512];

void set_path(struct sockaddr_un* addr, const char* name);

int ipc_init(void) {
    writeSocket = socket(DOMAIN, SOCK_DGRAM, 0);
    return writeSocket != -1;
}

int ipc_listen(const char* name) {

    struct sockaddr_un addr;

    pthread_mutex_lock(&readLock);
    readSocket = socket(DOMAIN, SOCK_DGRAM, 0);
    if (readSocket == -1) {
        pthread_mutex_unlock(&readLock);
        perror("Failed creating read socket");
        return -1;
    }

    addr.sun_family = DOMAIN;
    set_path(&addr, name);
    strcpy(path, addr.sun_path);

    if (bind(readSocket, (struct sockaddr*) &addr, sizeof(addr.sun_family) + strlen(addr.sun_path) + 2) == -1) {
        perror("Couldnt bind the read socket");
        pthread_mutex_unlock(&readLock);
        close(readSocket);
        return -1;
    }

    pthread_mutex_unlock(&readLock);
    return 0;
}

ipc_t ipc_establish(const char* name) {
    ipc_t conn = malloc(sizeof(struct ipc_t));
    struct stat statBuf;
    if (conn == NULL) {
        return NULL;
    }

    if (pthread_mutex_init(&conn->mutex, NULL) != 0) {
        free(conn);

        return NULL;
    }

    conn->addr.sun_family = DOMAIN;
    set_path(&conn->addr, name);

    while (stat(conn->addr.sun_path, &statBuf) == -1 && errno == ENOENT) {
        usleep(1000);
    }

    return conn;
}

int ipc_write(ipc_t conn, const void* buff, size_t len) {
    int res;

    pthread_mutex_lock(&conn->mutex);
    while ((res = sendto(writeSocket, buff, len, 0, (struct sockaddr*) &conn->addr, sizeof(struct sockaddr_un))) == -1 && errno == ENOBUFS) {
        usleep(1000);
    }
    if (res == -1) {
        perror("ipc_write");
    }
    pthread_mutex_unlock(&conn->mutex);

    return res;
}

int ipc_read(void* buff, size_t len) {
    int res;

    pthread_mutex_lock(&readLock);
    while ((res = read(readSocket, buff, len)) == -1 && errno == EINTR);
    if (res == -1) {
        perror("ipc_read");
    }
    pthread_mutex_unlock(&readLock);

    return res;
}

void ipc_close(ipc_t conn) {
    pthread_mutex_destroy(&conn->mutex);
    free(conn);
}

void ipc_end(void) {
    if (readSocket != -1) {
        close(readSocket);
    }
    close(writeSocket);
    unlink(path);
}

void set_path(struct sockaddr_un* addr, const char* name) {
    sprintf(addr->sun_path, "/tmp/ipc_sock_%s", name);
}

