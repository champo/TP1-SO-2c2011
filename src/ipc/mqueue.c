#include "ipc/ipc.h"
#include "util.h"
#include <fcntl.h> 
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_MSG    10 

char entryQueueName[512];
mqd_t entryQueueId;
pthread_mutex_t rd_mutex;

struct ipc_t {
    mqd_t mq_id;
    pthread_mutex_t wrt_mutex;
};

typedef struct msg_t {
    long type;
    char mtext[IPC_MAX_PACKET_LEN+1];
} msg_t;

int ipc_init(void) {
    return 0;
}

int ipc_listen(const char* name) {
    
    mqd_t mq_id; 
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_MSG;
    attr.mq_msgsize = sizeof(msg_t);
    
    pthread_mutex_init(&rd_mutex, NULL);
     
    sprintf(entryQueueName, "/%s", name);
    mq_id  = mq_open(entryQueueName, O_RDWR | O_CREAT, 0666, &attr); 
    
    if (mq_id == -1) {  
        print_errno("mq_open failed ");
        return -1;
    }

    entryQueueId = mq_id;
    return 0;
}

void ipc_end(void) {
    
    pthread_mutex_destroy(&rd_mutex);
    if (mq_unlink(entryQueueName) == -1) {
        print_errno("mq_unlink failed");
    }
}

ipc_t ipc_establish(const char* name) {

    char posix_name[512];
    ipc_t conn = malloc(sizeof(struct ipc_t));

    struct mq_attr attr;
    attr.mq_maxmsg = MAX_MSG;
    attr.mq_msgsize = sizeof(msg_t);
   
    pthread_mutex_init(&conn->wrt_mutex, NULL);
    
    sprintf(posix_name, "/%s", name);
    if ((conn->mq_id = mq_open(posix_name, O_WRONLY | O_CREAT, 0666, &attr)) == -1) {
        print_errno("mq_open failed");
        return NULL;
    }

    return conn;
}

void ipc_close(ipc_t conn) {
    
    if (mq_close(conn->mq_id) == -1) {
        print_errno("mq_close failed");
    }
    pthread_mutex_destroy(&conn->wrt_mutex);
    free(conn);
} 

int ipc_read(void* buff, size_t len) {
    int n;
    
    pthread_mutex_lock(&rd_mutex);
    if ((n = mq_receive(entryQueueId, buff, len > sizeof(msg_t)?len:sizeof(msg_t), NULL)) == -1) {
        print_errno("mq_receive failed");
    }
    pthread_mutex_unlock(&rd_mutex);
    return n;
}

int ipc_write(ipc_t conn, const void* buff, size_t len) {
    
    pthread_mutex_lock(&conn->wrt_mutex);
    if (mq_send(conn->mq_id, buff, len, 0) == -1) {
        print_errno("mq_send failed");
        return -1;
    }
    pthread_mutex_unlock(&conn->wrt_mutex);
    return len;
}

