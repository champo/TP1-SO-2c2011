#ifndef __COMM_AIRLINE__
#define __COMM_AIRLINE__

#include "marshall/map.h"

struct MapMessage {
    enum MapMessageType type;
    union {
        struct DestinationsMessage destinations;
    } payload;
};

int comm_airline_recieve(struct MapMessage* msg);

#endif
