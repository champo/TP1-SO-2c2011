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
    int cityId;
};

struct StockMessagePart {
    size_t count;
    int stockId[MAX_STOCKS];
    int quantities[MAX_STOCKS];
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

struct CheckDestinationsMessage marshall_check_destinations(int airline, int id, int cityId, Vector* stocks, size_t len);

struct StockStateMessage marshall_change_stock(int airline, int id, int cityId, Vector* stocks);

#endif
