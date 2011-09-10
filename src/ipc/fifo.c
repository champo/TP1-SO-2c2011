#include "ipc/ipc.h"
#include "util.h"

#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

struct ipc_t{
    int fd;
    pthread_mutex_t mutex;
};

int readfd = -1;
char path[512];

int ipc_init(void) {

    return 0;

}

int ipc_listen(const char* name) {
    
    sprintf(path, "/tmp/sim_%s", name);
    if (mkfifo(path, 0666)) {
        // If the fifo already exists, fine other may have created it.
        if ( errno != EEXIST) { 
            perror("mkfifo failed");
            return -1;
        }
    }
    
    readfd = open(path, O_RDWR);

    if (readfd == -1) {
        perror("open failed");
        return -1;
    }

    return 0;

}

ipc_t ipc_establish(const char* name) {
    
    ipc_t conn;
    char dest[512];

    sprintf(dest, "/tmp/sim_%s", name);
    if ((conn = malloc(sizeof(struct ipc_t))) == NULL) {
        return NULL;
    }

    if (mkfifo(dest, 0666)) {
        // If the fifo already exists, fine other may have created it.
        if ( errno != EEXIST) { 
            free(conn);
            perror("mkfifo failed");
            return NULL;
        }
    }
    
    conn->fd = open(dest, O_RDWR);

    if (conn->fd == -1) {
        free(conn);
        perror("open failed");
        return NULL;
    }


    if (pthread_mutex_init(&conn->mutex,NULL) != 0) {
        free(conn);
        return NULL;
    }

    return conn;

}

int ipc_write(ipc_t conn, const void* buff, size_t len) {
    int res;
    
    char tempbuf[IPC_MAX_PACKET_LEN + sizeof(size_t)];
    *((size_t*)tempbuf) = len <= IPC_MAX_PACKET_LEN ? len : IPC_MAX_PACKET_LEN;
    memcpy(tempbuf + sizeof(size_t), buff, (len <= IPC_MAX_PACKET_LEN ? len : IPC_MAX_PACKET_LEN));
    pthread_mutex_lock(&conn->mutex);
    res = write(conn->fd, tempbuf, (len <= IPC_MAX_PACKET_LEN ? len + sizeof(size_t): 
                IPC_MAX_PACKET_LEN + sizeof(size_t)));
    pthread_mutex_unlock(&conn->mutex);
    return res - sizeof(size_t);

}

int ipc_read(void* buff, size_t len) {
    
    size_t count;
    char tempbuf[IPC_MAX_PACKET_LEN];
    read(readfd, &count, sizeof(size_t));
    count = read(readfd,tempbuf,count);
    memcpy(buff, tempbuf, len);
    return (len < count? (int) len : count);

}

void ipc_close(ipc_t conn) {
    close(conn->fd);
    free(conn);

    return;
}

void ipc_end() {
    if (readfd != -1) {
        close(readfd);
        unlink(path);
    }

    return;
}
