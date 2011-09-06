#ifndef __MARSHALL_MAP__
#define __MARSHALL_MAP__

#include "global.h"
#include "models/city.h"
#include "communication/types.h"

struct DestinationsMessage {
    enum MessageType type;
    int planeId;
    size_t count;
    int destinations[MAX_DESTINATIONS];
    int distance[MAX_DESTINATIONS];
};

struct DestinationsMessage marshall_give_destinations(int planeId, int count, int* citiesIds, int* distances);

#endif
