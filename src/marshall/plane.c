#include "marshall/plane.h"
#include "models/stock.h"

struct SetDestinationMessage marshall_set_destination(int id, int target) {
    return (struct SetDestinationMessage) {.id = id, .target = target};
}

struct CheckDestinationsMessage marshall_check_destinations(int id, Vector* stocks, size_t len) {
    struct CheckDestinationsMessage msg;
    size_t count = getVectorSize(stocks);
    msg.id = id;
    msg.maxDestinations = len;
    msg.stocks.count = count;

    for (size_t i = 0; i < count; i++) {
        Stock* stock = (Stock*) getFromVector(stocks, i);
        msg.stocks.stockId[i] = stock->theShit->id;
        msg.stocks.quantities[i] = stock->amount;
    }

    return msg;
}

struct UnloadStockMessage marshall_unload_stock(int id, Vector* stocks) {
    struct UnloadStockMessage msg;
    size_t count = getVectorSize(stocks);
    msg.id = id;
    msg.stocks.count = count;

    for (size_t i = 0; i < count; i++) {
        Stock* stock = (Stock*) getFromVector(stocks, i);
        msg.stocks.stockId[i] = stock->theShit->id;
        msg.stocks.quantities[i] = stock->amount;
    }

    return msg;
}

