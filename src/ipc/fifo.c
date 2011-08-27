#include "ipc/ipc.h"

#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stddef.h>

struct ipc_t{
    int fd;
    pthread_mutex_t mutex;
};

int readfdi = -1;
char path[512];

int ipc_init(void) {

    return 0;

}

int ipc_listen(const char* name) {

    strcpy(path, name);
    if (mkfifo(path, O_CREAT | O666)) {
        return -1;
    }

    readfd = open(path, O_RDONLY);

    if (readfd == -1) {
        return -1;
    }

    return 0;

}

ipc_t ipc_establish(const char* name) {

    struct ipc_t conn;
    
    if ((conn = malloc(sizeof(struct ipc_t))) == NULL) {
        return NULL;
    }

    if (mkfifo(path, O_CREAT | O666)) {
        free(conn);
        return NULL;
    }
    
    &conn->fd = open(path, O_WRONLY);

    if (conn->fd == -1) {
        free(conn);
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
    *((int*)tempbuf) = len;
    memcpy(tempbuf + sizeof(size_t), buff, (len <= IPC_MAX_PACKET_LEN ? len : IPC_MAX_PACKET_LEN));
    pthread_mutex_lock(&conn->mutex);
    res = write(conn->fd, tempbuf, (len <= IPC_MAX_PACKET_LEN ? len + sizeof(size_t): 
                IPC_MAX_PACKET_LEN + sizeof(size_t)));
    pthread_mutex_unlock(&conn->mutex);

    return res;

}

int ipc_read(void* buff, size_t len) {
    
    int count;
    char tempbuf[IPC_MAX_PACKET_LEN];
    read(readfd,tempbuf,sizeof(size_t));
    
    count = atoi(tempbuf);
    count = read(readfd,tempbuf,count);
    memcpy(buff, tempbuf, len);
    return (len < count? len : count);
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
