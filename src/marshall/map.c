#include "models/city.h"
#include "marshall/map.h"
#include <stddef.h>



struct DestinationsMessage marshall_give_destinations(int planeId, int count, City* cities, int* distances) {
    
    struct DestinationsMessage msg;
    
    msg.type = DestinationMessageType;
    msg.planeId = planeId;
    msg.count = count;

    for (size_t i = 0; i < msg.count; i++) {
        msg.destinations[i] = cities[i].id;
        msg.distance[i] = distances[i];
    }
    return msg;
}
