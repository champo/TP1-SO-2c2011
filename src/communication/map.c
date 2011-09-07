#include "ipc/ipc.h"
#include "models/plane.h"
#include "models/city.h"
#include "models/map.h"
#include "communication/map.h"
#include "communication/plane.h"
#include "marshall/map.h"
#include "marshall/plane.h"
#include "communication/msgqueue.h"
#include "ipc/ipc.h"
#include "communication/types.h"

#include <string.h>
int comm_unloaded_stock(int airlineID, Plane* plane, ipc_t conn){

    struct StockStateMessage msg;
    msg = marshall_change_stock(airlineID, plane->id, plane->cityId, plane->stocks);
    transmit(conn, msg);

    return 0;
}

int comm_give_destinations(Plane* plane, ipc_t conn, int count, int* citiesIds, int* distances){

    struct DestinationsMessage msg;
    msg = marshall_give_destinations(plane->id, count, citiesIds, distances);
    transmit(conn, msg);

    return 0;
}

int comm_turn_step(Vector* conns) {

    enum MessageType msg = MessageTypeStep;
    unsigned i;

    for (i = 0; i < getVectorSize(conns); i++) {
        transmit(getFromVector(conns, i), msg);
    }
    return 0;
}

int comm_turn_continue(Vector* conns) {

    enum MessageType msg = MessageTypeContinue;
    unsigned i;

    for (i = 0; i < getVectorSize(conns); i++) {
        transmit(getFromVector(conns, i), msg);
    }

    return 0;
}

int comm_get_map_message(union MapMessage* msg) {
    return ipc_read(msg, sizeof(union MapMessage));
}

