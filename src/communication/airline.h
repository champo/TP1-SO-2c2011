#ifndef __COMM_AIRLINE__
#define __COMM_AIRLINE__

#include "marshall/map.h"
#include "ipc/ipc.h"
#include "communication/types.h"
#include "marshall/plane.h"
#include "models/airline.h"

union MapMessage {
    enum MessageType type;
    struct DestinationsMessage destinations;
    struct StockStateMessage stock;
};

int comm_airline_recieve(union MapMessage* msg);

int comm_airline_ready(ipc_t conn);

int comm_airline_status(ipc_t conn, int id, int flying, int total);

#endif
