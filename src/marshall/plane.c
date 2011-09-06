#include "marshall/plane.h"
#include "models/stock.h"

static struct PlaneMessageHeader prepare_header(int airline, int plane, enum MessageType type);

struct PlaneMessageHeader prepare_header(int airline, int plane, enum MessageType type) {
    return (struct PlaneMessageHeader) {
        .type = type,
        .id = plane,
        .airline = airline
    };
}

struct CheckDestinationsMessage marshall_check_destinations(int airline, int id, Vector* stocks, size_t len) {
    struct CheckDestinationsMessage msg;
    size_t count = getVectorSize(stocks);
    msg.header = prepare_header(airline, id, MessageTypeCheckDestinations);
    msg.maxDestinations = len;
    msg.stocks.count = count;

    for (size_t i = 0; i < count; i++) {
        Stock* stock = (Stock*) getFromVector(stocks, i);
        msg.stocks.stockId[i] = stock->theShit->id;
        msg.stocks.quantities[i] = stock->amount;
    }

    return msg;
}

struct StockStateMessage marshall_change_stock(int airline, int id, Vector* stocks) {
    struct StockStateMessage msg;
    size_t count = getVectorSize(stocks);
    msg.header = prepare_header(airline, id, MessageTypeUnloadStock);
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
        .header = prepare_header(airline, id, MessageTypeInTransit)
    };
}

