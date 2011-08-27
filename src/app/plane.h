#ifndef __APP_PLANE__
#define __APP_PLANE__

#include <pthread.h>
#include "utils/msgqueue.h"
#include "ipc/ipc.h"
#include "models/plane.h"

struct PlaneThread {
    pthread_t thread;
    struct MessageQueue* queue;
    ipc_t conn;
    Plane* plane;
    int done;
};

void run_plane(struct PlaneThread* self);

#endif
