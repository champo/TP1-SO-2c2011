#include "communication/msgqueue.h"

#include <pthread.h>
#include <stdlib.h>

#define QUEUE_SIZE 10

struct MessageQueue {
    pthread_mutex_t mutex;
    pthread_cond_t write;
    pthread_cond_t read;
    struct Message messages[QUEUE_SIZE];
    int first;
    int last;
};

struct MessageQueue* message_queue_create(void) {
    struct MessageQueue* queue = malloc(sizeof(struct MessageQueue));
    if (queue == NULL) {
        return NULL;
    }

    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->write, NULL);
    pthread_cond_init(&queue->read, NULL);

    queue->first = 0;
    queue->last = 0;

    return queue;
}

struct Message message_queue_pop(struct MessageQueue* queue) {

    struct Message msg;

    pthread_mutex_lock(&queue->mutex);
    while (queue->first == queue->last) {
        pthread_cond_wait(&queue->write, &queue->mutex);
    }
    msg = queue->messages[queue->first];
    queue->first = (queue->first + 1) % QUEUE_SIZE;
    pthread_cond_signal(&queue->read);
    pthread_mutex_unlock(&queue->mutex);

    return msg;
}

void message_queue_push(struct MessageQueue* queue, struct Message msg) {

    pthread_mutex_lock(&queue->mutex);
    while ((queue->last + 1) % QUEUE_SIZE == queue->first) {
        pthread_cond_wait(&queue->read, &queue->mutex);
    }

    queue->last = (queue->last + 1) % QUEUE_SIZE;
    queue->messages[queue->last] = msg;

    pthread_cond_signal(&queue->write);
    pthread_mutex_unlock(&queue->mutex);
}

void message_queue_destroy(struct MessageQueue* queue) {

    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->read);
    pthread_cond_destroy(&queue->write);

    free(queue);
}
