#ifndef __COMM_MAP__
#define __COMM_MAP__

#include "utils/vector.h"
#include "marshall/plane.h"
#include "communication/types.h"

struct PlaneMessage {
    enum MessageType type;
    union {
        struct SetDestinationMessage setDestination;
        struct CheckDestinationsMessage checkDestinations;
        struct StockStateMessage stockState;
        struct InTransitMessage inTransit;
    } planeInfo;
};

int comm_turn_step(Vector* conns);

int comm_turn_continue(Vector* conns);

int comm_unloaded_stock(int airlineID, Plane* plane, ipc_t conn);

int comm_get_map_message(struct PlaneMessage* msg);

int comm_give_destinations(Plane* plane, ipc_t conn, int count, int* citiesIds, int* distances);

#endif
