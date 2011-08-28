#ifndef __MARSHALL_PLANE__
#define __MARSHALL_PLANE__

#include "global.h"
#include <stddef.h>
#include "utils/vector.h"

struct StockMessagePart {
    size_t count;
    int stockId[MAX_STOCKS];
    int quantities[MAX_STOCKS];
};

struct SetDestinationMessage {
    int id;
    int target;
};

struct CheckDestinationsMessage {
    int id;
    size_t maxDestinations;
    struct StockMessagePart stocks;
};

struct UnloadStockMessage {
    int id;
    struct StockMessagePart stocks;
};

struct SetDestinationMessage marshall_set_destination(int id, int target);

struct CheckDestinationsMessage marshall_check_destinations(int id, Vector* stocks, size_t len);

struct UnloadStockMessage marshall_unload_stock(int id, Vector* stocks);

#endif
