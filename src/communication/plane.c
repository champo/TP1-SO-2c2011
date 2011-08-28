#include "communication/plane.h"
#include "communication/msgqueue.h"
#include "ipc/ipc.h"

int comm_check_destinations(struct PlaneThread* plane, int* destinations, size_t* len) {
}

int comm_set_destination(struct PlaneThread* plane, int target) {

    struct SetDestinationMessage msg = marshall_set_destination(plane->plane->id, target);
    ipc_write(plane->conn, &msg, sizeof(msg));

    return 0;
}

int comm_continue(struct PlaneThread* plane) {
    struct Message msg = message_queue_pop(plane->queue);
    return msg.type != MessageTypeContinue ? 0 : -1;
}

int comm_step(struct PlaneThread* plane) {
}

