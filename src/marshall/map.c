#include "models/city.h"
#include "marshall/map.h"
#include <stddef.h>



struct DestinationsMessage marshall_give_destinations(int planeId, int count, int* citiesIds, int* distances) {

    struct DestinationsMessage msg;

    msg.type = MessageTypeDestinations;
    msg.planeId = planeId;
    msg.count = count;

    for (size_t i = 0; i < msg.count; i++) {
        msg.destinations[i] = citiesIds[i];
        msg.distance[i] = distances[i];
    }
    return msg;
}
