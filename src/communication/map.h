#ifndef __COMM_MAP__
#define __COMM_MAP__

#include "utils/vector.h"
#include "marshall/plane.h"

struct MapMessage {
    enum PlaneMessageType type;
    union PlaneInfo {
        Plane plane;
        int airlineID;
    } planeInfo;
};

int comm_turn_step(Vector* conns);
int comm_turn_continue(Vector* conns);
int comm_unloaded_stock(int airlineID, Plane* plane, ipc_t conn);

int comm_give_destinations(Plane* plane, ipc_t conn, int cityNumber, City* cities, int* distances);
#endif
