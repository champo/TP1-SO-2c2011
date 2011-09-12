#ifndef __MARSHALL_MAP__
#define __MARSHALL_MAP__

#include "global.h"
#include "models/city.h"
#include "communication/types.h"
#include "communication/msgqueue.h"

struct DestinationsMessage {
    enum MessageType type;
    int planeId;
    size_t count;
    int destinations[MAX_DESTINATIONS];
    int distance[MAX_DESTINATIONS];
};

struct DestinationsMessage marshall_give_destinations(int planeId, int count, int* citiesIds, int* distances);

struct Message marshall_send_map_status(double completionPercentage, int citiesSatisfied, int totalCities);

struct AirlineStatus marshall_send_airline_status(int planesFlying, int totalPlanes, int id);

struct Message marshall_end_output(void);

#endif
