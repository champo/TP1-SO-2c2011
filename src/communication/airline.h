#ifndef __COMM_AIRLINE__
#define __COMM_AIRLINE__

#include "marshall/map.h"
#include "ipc/ipc.h"
#include "communication/types.h"
#include "marshall/plane.h"

struct MapMessage {
    enum MessageType type;
    union {
        struct DestinationsMessage destinations;
        struct StockStateMessage stock;
    } payload;
};

int comm_airline_recieve(struct MapMessage* msg);

int comm_airline_ready(ipc_t conn);

#endif