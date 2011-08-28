#include "app/airline.h"

#include <pthread.h>
#include <stdlib.h>

#include "app/signal.h"
#include "communication/plane.h"
#include "utils/vector.h"
#include "app/plane.h"

static void signal_handler(void);

static void listen(Vector* conns);

static Vector* bootstrap_planes(Airline* self, ipc_t conn);

static void (*parent_cleanup)(void) = NULL;

static pthread_cond_t exitWait = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t resourcesLock = PTHREAD_MUTEX_INITIALIZER;

static int exitState = 0;

void run_airline(Airline* self, ipc_t conn) {

    pthread_mutex_lock(&resourcesLock);
    register_exit_function(signal_handler);

    pthread_t listenerThread;
    Vector* threads = bootstrap_planes(self, conn);

    pthread_create(&listenerThread, NULL, (void*(*)(void*))listen, threads);

    while (exitState == 0) {
        pthread_cond_wait(&exitWait, &resourcesLock);
    }

    // If we got here, it means we recieved an end message
    for (size_t i = 0; i < self->numberOfPlanes; i++) {
        struct PlaneThread* t = getFromVector(threads, i);
        //TODO: Broadcast end message
        pthread_join(t->thread, NULL);
        message_queue_destroy(t->queue);
        free(t);
    }

    destroyVector(threads);
    pthread_mutex_unlock(&resourcesLock);
}

Vector* bootstrap_planes(Airline* self, ipc_t conn) {
    struct PlaneThread* t;
    Vector* threads = createVector();
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (size_t i = 0; i < self->numberOfPlanes; i++) {
        t = malloc(sizeof(struct PlaneThread));
        t->queue = message_queue_create();
        t->plane = &self->planes[i];
        t->conn = conn;
        t->done = 0;
        pthread_create(&t->thread, &attr, (void*(*)(void*))run_plane, t);
        addToVector(threads, t);
    }

    pthread_attr_destroy(&attr);

    return threads;
}

void listen(Vector* threads) {
    char buff[IPC_MAX_PACKET_LEN];
    //FIXME: Add a transport layer
    while (ipc_read(buff, IPC_MAX_PACKET_LEN) > 0) {
        //TODO: Process this
    }
}

void signal_handler(void) {
    pthread_mutex_lock(&resourcesLock);
    exitState = 1;
    pthread_cond_signal(&exitWait);
    pthread_mutex_unlock(&resourcesLock);
}

