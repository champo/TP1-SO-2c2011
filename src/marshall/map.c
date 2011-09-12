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


struct Message marshall_send_map_status(double completionPercentage, int citiesSatisfied, int totalCities) {

    struct Message msg;

    msg.type = MessageTypeMapStatus;
    msg.payload.mapStatus.completionPercentage = completionPercentage;
    msg.payload.mapStatus.citiesSatisfied = citiesSatisfied;
    msg.payload.mapStatus.totalCities = totalCities;

    return msg;
}

struct AirlineStatus marshall_send_airline_status(int planesFlying, int totalPlanes, int id) {

    struct AirlineStatus msg;

    msg.planesFlying = planesFlying;
    msg.totalPlanes = totalPlanes;
    msg.id = id;

    return msg;
}

struct Message marshall_end_output() {
    struct Message msg;
    msg.type = MessageTypeEndOutput;
    return msg;
}
