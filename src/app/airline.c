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

static void redirect_destinations_message(Vector* threads, struct MapMessage* in);

static void redirect_stock_message(Vector* threads, struct MapMessage* in);

static void set_planes_left(Vector* threads);

static pthread_cond_t exitWait = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t resourcesLock = PTHREAD_MUTEX_INITIALIZER;

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
        t->airline = self->id;
        pthread_create(&t->thread, &attr, (void*(*)(void*))run_plane, t);
        addToVector(threads, t);
    }

    pthread_attr_destroy(&attr);

    return threads;
}

void listen(Vector* threads) {

    struct MapMessage msg;
    struct Message outMsg;

    while (comm_airline_recieve(&msg) == 0) {
        mprintf("Got message with type %d\n", msg.type);
        switch (msg.type) {
            case MessageTypeStep:
                set_planes_left(threads);
                if (planesLeftInStage == 0) {
                    comm_airline_ready(ipcConn);
                } else {
                    outMsg.type = MessageTypeStep;
                    broadcast(threads, outMsg);
                }
                break;
            case MessageTypeContinue:
                set_planes_left(threads);
                if (planesLeftInStage == 0) {
                    comm_airline_ready(ipcConn);
                } else {
                    outMsg.type = MessageTypeContinue;
                    broadcast(threads, outMsg);
                }
                break;
            case MessageTypeDestinations:
                redirect_destinations_message(threads, &msg);
                break;
            case MessageTypeUnloadStock:
                redirect_stock_message(threads, &msg);
                break;
            default:
                exit_handler();
                return;
        }
    }
}

void redirect_stock_message(Vector* threads, struct MapMessage* in) {
    struct Message msg;
    struct PlaneThread* thread = (struct PlaneThread*) getFromVector(threads, in->payload.stock.header.id);
    struct StockMessagePart* stock = &in->payload.stock.stocks;

    msg.type = MessageTypeStock;
    msg.payload.stock.count = stock->count;
    memcpy(msg.payload.stock.delta, stock->quantities, sizeof(int) * MAX_STOCKS);

    mprintf("Redirect stock message to %d\n", thread->plane->id);
    message_queue_push(thread->queue, msg);
}

void redirect_destinations_message(Vector* threads, struct MapMessage* in) {

    struct Message msg;
    struct PlaneThread* thread = (struct PlaneThread*) getFromVector(threads, in->payload.destinations.planeId);

    msg.type = MessageTypeDestinations;
    memcpy(msg.payload.destinations.destinations, in->payload.destinations.destinations, MAX_DESTINATIONS * sizeof(int));
    memcpy(msg.payload.destinations.distances, in->payload.destinations.distance, MAX_DESTINATIONS * sizeof(int));
    msg.payload.destinations.count = in->payload.destinations.count;

    message_queue_push(thread->queue, msg);
}

void exit_handler(void) {
    pthread_mutex_lock(&resourcesLock);
    exitState = 1;
    pthread_cond_signal(&exitWait);
    pthread_mutex_unlock(&resourcesLock);
}

void broadcast(Vector* threads, struct Message msg) {
    mprintf("Broadcasting message with type %d\n", msg.type);
    size_t len = getVectorSize(threads);
    for (size_t i = 0; i < len; i++) {
        struct PlaneThread* plane = (struct PlaneThread*) getFromVector(threads, i);
        message_queue_push(plane->queue, msg);
    }
}

void set_planes_left(Vector* threads) {
    size_t len = getVectorSize(threads);
    planesLeftInStage = 0;
    for (size_t i = 0; i < len; i++) {
        struct PlaneThread* thread = (struct PlaneThread*) getFromVector(threads, i);
        if (!thread->done) {
            planesLeftInStage++;
        }
    }
    mprintf("Setting planes left to %d\n", planesLeftInStage);
}

void app_airline_plane_ready(void) {
    planesLeftInStage--;
    mprintf("Plane ready, %d left\n", planesLeftInStage);
    if (planesLeftInStage == 0) {
        comm_airline_ready(ipcConn);
    }
}

