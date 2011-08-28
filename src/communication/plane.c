#include "communication/plane.h"
#include "communication/msgqueue.h"
#include "ipc/ipc.h"
#include "marshall/plane.h"

#include <string.h>

#define send(plane, msg) if (sizeof(msg) != ipc_write((plane->conn), &(msg), sizeof(msg))) { \
        return -1; \
        }

int comm_check_destinations(struct PlaneThread* plane, int* destinations, size_t* len) {

    Plane* model = plane->plane;
    struct CheckDestinationsMessage msg;
    struct Message res;
    union MessagePayload* payload;

    msg = marshall_check_destinations(model->id, model->stocks, *len);
    send(plane, msg);

    res = message_queue_pop(plane->queue);
    if (res.type != MessageTypeDestinations) {
        return -1;
    }

    payload = &res.payload;
    if (*len > payload->destinations.count) {
        *len = payload->destinations.count;
    }
    memcpy(destinations, payload->destinations.destinations, (*len) * sizeof(int));

    return 0;
}

int comm_set_destination(struct PlaneThread* plane, int target) {
    struct SetDestinationMessage msg = marshall_set_destination(plane->plane->id, target);
    send(plane, msg);
    return 0;
}

int comm_continue(struct PlaneThread* plane) {
    struct Message msg = message_queue_pop(plane->queue);
    return msg.type != MessageTypeContinue ? 0 : -1;
}

int comm_step(struct PlaneThread* plane) {
    struct Message msg = message_queue_pop(plane->queue);
    return (msg.type != MessageTypeStep) ? -1 : 0;
}

int comm_unload_stock(struct PlaneThread* plane, int* stockDelta) {

    Plane* model = plane->plane;
    struct Message res;
    struct UnloadStockMessage msg;

    msg = marshall_unload_stock(model->id, model->stocks);
    send(plane, msg);

    res = message_queue_pop(plane->queue);
    if (res.type != MessageTypeStock) {
        return -1;
    }

    memcpy(stockDelta, res.payload.stock.delta, sizeof(int) * res.payload.stock.count);
    return 0;
}

