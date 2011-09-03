#ifndef __MARSHALL_MAP__
#define __MARSHALL_MAP__

#include "global.h"

struct DestinationsMessage {
    int planeId;
    size_t count;
    int destinations[MAX_DESTINATIONS];
    int distance[MAX_DESTINATIONS];
};

enum MapMessageType {
    StartPhaseTwo,
    StartPhaseOne
};

struct DestinationsMessage marshall_give_destinations(int planeId, int cityNumber, City* cities, int* distances); 
#endif

