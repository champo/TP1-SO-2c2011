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

static void listen(Vector* conns);

static Vector* bootstrap_planes(Airline* self, ipc_t conn);

static void broadcast(Vector* threads, struct Message msg);

static void redirect_destinations_message(Vector* threads, union MapMessage* in);

static void redirect_stock_message(Vector* threads, union MapMessage* in);

static void start_phase(Vector* threads, struct Message msg);

static pthread_mutex_t planesLeftLock = PTHREAD_MUTEX_INITIALIZER;

static int exitState = 0;

static int planesLeftInStage;

static int stage = 0;

static ipc_t ipcConn;

static Airline* me;

static Vector* planeThreads;

void run_airline(Airline* self, ipc_t conn) {

    mprintf("Dude\n");
    ipcConn = conn;
    me = self;

    register_exit_function(NULL);
    //redirect_signals();

    Vector* threads = bootstrap_planes(self, conn);
    planeThreads = threads;

    listen(threads);

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
    mprintf("Out of run_airline\n");
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
                stage = 0;
                outMsg.type = MessageTypeStep;
                start_phase(threads, outMsg);
                break;
            case MessageTypeContinue:
                stage = 1;
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
                if (msg.type >= MessageTypeLast) {
                    print_error("Got invalid message type on airline listen\n");
                }
                return;
        }
        mprintf("Done handling. Waiting for next msg\n");
    }
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
    if (planesLeftInStage == 0 && exitState != 1) {

        if (stage == 1) {

            int flying = 0;
            for (size_t i = 0; i < me->numberOfPlanes; i++) {
                struct PlaneThread* thread = getFromVector(planeThreads, i);
                if (!thread->done) {
                    flying++;
                }
            }

            comm_airline_status(ipcConn, me->id, flying, me->numberOfPlanes);
        } else {
            comm_airline_ready(ipcConn);
        }
    }

    pthread_mutex_unlock(&planesLeftLock);
}

