#ifndef __MARSHALL_PLANE__
#define __MARSHALL_PLANE__

#include "global.h"

struct SetDestinationMessage {
    int id;
    int target;
};

struct CheckDestinationsMessage {
    int id;
    int destinations[MAX_DESTINATIONS];
};

struct UnloadStockMessage {
    int id;
    size_t count;
    int stockId[MAX_STOCKS];
    int quantities[MAX_STOCKS];
};

struct SetDestinationMessage marshall_set_destination(int id, int target);

struct CheckDestinationsMessage marshall_check_destinations(int id, Vector* stocks, size_t len);

struct UnloadStockMessage marshall_unload_stock(int id, Vector* stocks);

#endif
