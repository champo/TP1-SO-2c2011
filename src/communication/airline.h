#ifndef __COMM_AIRLINE__
#define __COMM_AIRLINE__

#include "marshall/map.h"
#include "ipc/ipc.h"

struct MapMessage {
    enum MapMessageType type;
    union {
        struct DestinationsMessage destinations;
    } payload;
};

int comm_airline_recieve(struct MapMessage* msg);

int comm_airline_ready(ipc_t conn);

#endif
