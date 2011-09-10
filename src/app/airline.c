#include "app/airline.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "app/signal.h"
#include "communication/plane.h"
#include "communication/airline.h"
#include "utils/vector.h"
#include "app/plane.h"
#include "global.h"

static void exit_handler(void);

static void listen(Vector* conns);

static Vector* bootstrap_planes(Airline* self, ipc_t conn);

static void broadcast(Vector* threads, struct Message msg);

static void redirect_destinations_message(Vector* threads, union MapMessage* in);

static void redirect_stock_message(Vector* threads, union MapMessage* in);

static void start_phase(Vector* threads, struct Message msg);

static pthread_cond_t exitWait = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t resourcesLock = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t planesLeftLock = PTHREAD_MUTEX_INITIALIZER;

static int exitState = 0;

static int planesLeftInStage;

static ipc_t ipcConn;

void run_airline(Airline* self, ipc_t conn) {

    mprintf("Dude\n");
    ipcConn = conn;

    pthread_mutex_lock(&resourcesLock);
    register_exit_function(exit_handler);

    pthread_t listenerThread;
    Vector* threads = bootstrap_planes(self, conn);

    pthread_create(&listenerThread, NULL, (void*(*)(void*))listen, threads);

    while (exitState == 0) {
        pthread_cond_wait(&exitWait, &resourcesLock);
    }

    // If we got here, it means we recieved an end message
    struct Message msg;
    msg.type = MessageTypeEnd;
    broadcast(threads, msg);

    for (size_t i = 0; i < self->numberOfPlanes; i++) {
        struct PlaneThread* t = getFromVector(threads, i);
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
        t->airline = self->id;
        pthread_create(&t->thread, &attr, (void*(*)(void*))run_plane, t);
        addToVector(threads, t);
    }

    pthread_attr_destroy(&attr);

    return threads;
}

void listen(Vector* threads) {

    union MapMessage msg;
    struct Message outMsg;

    mprintf("Starting listen loop\n");
    while (comm_airline_recieve(&msg) == 0) {
        mprintf("Got message with type %d\n", msg.type);
        switch (msg.type) {
            case MessageTypeStep:
                outMsg.type = MessageTypeStep;
                start_phase(threads, outMsg);
                break;
            case MessageTypeContinue:
                outMsg.type = MessageTypeContinue;
                start_phase(threads, outMsg);
                break;
            case MessageTypeDestinations:
                redirect_destinations_message(threads, &msg);
                break;
            case MessageTypeUnloadStock:
                redirect_stock_message(threads, &msg);
                break;
            default:
                if (msg.type >= MessageTypeLast || msg.type < 0) {
                    print_error("Got invalid message type on airline listen\n");
                }

                exit_handler();
                pthread_exit(0);
                return;
        }
        mprintf("Done handling. Waiting for next msg\n");
    }
    print_error("HELL\n");
}

void redirect_stock_message(Vector* threads, union MapMessage* in) {
    struct Message msg;
    struct PlaneThread* thread = (struct PlaneThread*) getFromVector(threads, in->stock.header.id);
    struct StockMessagePart* stock = &in->stock.stocks;

    msg.type = MessageTypeStock;
    msg.payload.stock.count = stock->count;
    memcpy(msg.payload.stock.delta, stock->quantities, sizeof(int) * MAX_STOCKS);

    mprintf("Redirect stock message to %d with stock count %d\n", thread->plane->id, stock->count);
    message_queue_push(thread->queue, msg);
}

void redirect_destinations_message(Vector* threads, union MapMessage* in) {

    struct Message msg;
    struct PlaneThread* thread = (struct PlaneThread*) getFromVector(threads, in->destinations.planeId);

    msg.type = MessageTypeDestinations;
    memcpy(msg.payload.destinations.destinations, in->destinations.destinations, MAX_DESTINATIONS * sizeof(int));
    memcpy(msg.payload.destinations.distances, in->destinations.distance, MAX_DESTINATIONS * sizeof(int));
    msg.payload.destinations.count = in->destinations.count;

    message_queue_push(thread->queue, msg);
}

void exit_handler(void) {
    exitState = 1;
    pthread_cond_signal(&exitWait);
}

void broadcast(Vector* threads, struct Message msg) {
    mprintf("Broadcasting message with type %d\n", msg.type);
    size_t len = getVectorSize(threads);
    for (size_t i = 0; i < len; i++) {
        struct PlaneThread* plane = (struct PlaneThread*) getFromVector(threads, i);
        if (!plane->done) {
            message_queue_push(plane->queue, msg);
        }
    }
}

void start_phase(Vector* threads, struct Message msg) {

    pthread_mutex_lock(&planesLeftLock);
    size_t len = getVectorSize(threads);
    planesLeftInStage = 0;
    for (size_t i = 0; i < len; i++) {
        struct PlaneThread* thread = (struct PlaneThread*) getFromVector(threads, i);
        if (!thread->done) {
            planesLeftInStage++;
        }
    }
    mprintf("Setting planes left to %d\n", planesLeftInStage);

    if (planesLeftInStage == 0) {
        comm_airline_ready(ipcConn);
    } else {
        broadcast(threads, msg);
    }
    pthread_mutex_unlock(&planesLeftLock);
}

void app_airline_plane_ready(void) {
    pthread_mutex_lock(&planesLeftLock);
    planesLeftInStage--;
    mprintf("Plane ready, %d left\n", planesLeftInStage);
    if (planesLeftInStage == 0) {
        comm_airline_ready(ipcConn);
    }
    pthread_mutex_unlock(&planesLeftLock);
}

