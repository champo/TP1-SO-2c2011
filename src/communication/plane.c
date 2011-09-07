#include "communication/plane.h"
#include "communication/msgqueue.h"
#include "ipc/ipc.h"
#include "marshall/plane.h"

#include <string.h>

#define send(plane, msg) transmit((plane->conn), (msg))

int comm_check_destinations(struct PlaneThread* plane, int* destinations, int* distances, size_t* len) {

    Plane* model = plane->plane;
    struct CheckDestinationsMessage msg;
    struct Message res;
    union MessagePayload* payload;

    msg = marshall_check_destinations(plane->airline, model->id, plane->plane->cityId, model->stocks, *len);
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
    memcpy(distances, payload->destinations.distances, (*len) * sizeof(int));

    return 0;
}

int comm_continue(struct PlaneThread* plane) {
    struct Message msg = message_queue_pop(plane->queue);
    return msg.type != MessageTypeContinue ? -1 : 0;
}

int comm_step(struct PlaneThread* plane) {
    struct Message msg = message_queue_pop(plane->queue);
    return (msg.type != MessageTypeStep) ? -1 : 0;
}

int comm_unload_stock(struct PlaneThread* plane, int* stockDelta) {

    Plane* model = plane->plane;
    struct Message res;
    struct StockStateMessage msg;

    msg = marshall_change_stock(plane->airline, model->id, plane->plane->cityId, model->stocks);
    send(plane, msg);

    res = message_queue_pop(plane->queue);
    if (res.type != MessageTypeStock) {
        return -1;
    }

    memcpy(stockDelta, res.payload.stock.delta, sizeof(int) * res.payload.stock.count);
    return 0;
}

