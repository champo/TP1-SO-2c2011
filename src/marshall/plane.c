#include "marshall/plane.h"
#include "models/stock.h"

static struct PlaneMessageHeader prepare_header(int airline, int plane, enum PlaneMessageType type);

struct PlaneMessageHeader prepare_header(int airline, int plane, enum PlaneMessageType type) {
    return (struct PlaneMessageHeader) {
        .type = type,
        .id = plane,
        .airline = airline
    };
}

struct SetDestinationMessage marshall_set_destination(int airline, int id, int target) {
    return (struct SetDestinationMessage) {
        .header = prepare_header(airline, id, SetDestinationType),
        .target = target
    };
}

struct CheckDestinationsMessage marshall_check_destinations(int airline, int id, Vector* stocks, size_t len) {
    struct CheckDestinationsMessage msg;
    size_t count = getVectorSize(stocks);
    msg.header = prepare_header(airline, id, CheckDestinationsType);
    msg.maxDestinations = len;
    msg.stocks.count = count;

    for (size_t i = 0; i < count; i++) {
        Stock* stock = (Stock*) getFromVector(stocks, i);
        msg.stocks.stockId[i] = stock->theShit->id;
        msg.stocks.quantities[i] = stock->amount;
    }

    return msg;
}

struct ChangeStockMessage marshall_change_stock(int airline, int id, Vector* stocks) {
    struct ChangeStockMessage msg;
    size_t count = getVectorSize(stocks);
    msg.header = prepare_header(airline, id, UnloadStockType);
    msg.stocks.count = count;

    for (size_t i = 0; i < count; i++) {
        Stock* stock = (Stock*) getFromVector(stocks, i);
        msg.stocks.stockId[i] = stock->theShit->id;
        msg.stocks.quantities[i] = stock->amount;
    }

    return msg;
}

struct InTransitMessage marshall_intransit(int airline, int id) {
    return (struct InTransitMessage) {
        .header = prepare_header(airline, id, InTransitType)
    };
}

