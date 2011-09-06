#ifndef __MARSHALL_PLANE__
#define __MARSHALL_PLANE__

#include "global.h"
#include <stddef.h>
#include "utils/vector.h"
#include "communication/types.h"

struct PlaneMessageHeader {
    enum MessageType type;
    int airline;
    int id;
};

struct StockMessagePart {
    size_t count;
    int stockId[MAX_STOCKS];
    int quantities[MAX_STOCKS];
};

struct SetDestinationMessage {
    struct PlaneMessageHeader header;
    int target;
};

struct CheckDestinationsMessage {
    struct PlaneMessageHeader header;
    size_t maxDestinations;
    struct StockMessagePart stocks;
};

struct StockStateMessage {
    struct PlaneMessageHeader header;
    struct StockMessagePart stocks;
};

struct InTransitMessage {
    struct PlaneMessageHeader header;
};

struct SetDestinationMessage marshall_set_destination(int airline, int id, int target);

struct CheckDestinationsMessage marshall_check_destinations(int airline, int id, Vector* stocks, size_t len);

struct InTransitMessage marshall_intransit(int airline, int id);

struct StockStateMessage marshall_change_stock(int airline, int id, Vector* stocks);

#endif
