#ifndef __COMM_MAP__
#define __COMM_MAP__

#include "utils/vector.h"
#include "marshall/plane.h"
#include "communication/types.h"
#include "communication/msgqueue.h"

union PlaneMessage {
    enum MessageType type;
    struct CheckDestinationsMessage checkDestinations;
    struct StockStateMessage stockState;
    struct AirlineStatus airlineStatus;
};


int comm_turn_step(Vector* conns);

int comm_turn_continue(Vector* conns);

int comm_unloaded_stock(int airlineID, Plane* plane, ipc_t conn);

int comm_get_map_message(union PlaneMessage* msg);

int comm_give_destinations(Plane* plane, ipc_t conn, int count, int* citiesIds, int* distances);

int comm_end(Vector* conns);

int comm_send_map_status(double completionPercentage, int citiesSatisfied, int totalCities, struct MessageQueue* outputMsgQueue);

int comm_send_airline_status(int planesFlying, int totalPlanes, int id, struct MessageQueue* outputMsgQueue);

int comm_end_output(struct MessageQueue* outputMsgQueue); 
#endif
