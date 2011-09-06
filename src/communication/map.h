#ifndef __COMM_MAP__
#define __COMM_MAP__

#include "utils/vector.h"
#include "marshall/plane.h"
#include "communication/types.h"

struct MapMessage {
    enum MessageType type;
    union PlaneInfo {
        Plane plane;
        int airlineID;
    } planeInfo;
};

int comm_turn_step(Vector* conns);

int comm_turn_continue(Vector* conns);

int comm_unloaded_stock(int airlineID, Plane* plane, ipc_t conn);
struct MapMessage comm_get_map_message(void);

int comm_give_destinations(Plane* plane, ipc_t conn, int count, int* citiesIds, int* distances);

#endif
