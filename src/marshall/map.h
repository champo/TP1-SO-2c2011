#ifndef __MARSHALL_MAP__
#define __MARSHALL_MAP__

#include "global.h"
#include "models/city.h"

enum MapMessageType {
        StartPhaseOne,
        StartPhaseTwo,
        Destination  
};

struct DestinationsMessage {
    enum MapMessageType type;
    int planeId;
    size_t count;
    int destinations[MAX_DESTINATIONS];
    int distance[MAX_DESTINATIONS];
};

struct DestinationsMessage marshall_give_destinations(int planeId, int count, City* cities, int* distances); 

#endif

