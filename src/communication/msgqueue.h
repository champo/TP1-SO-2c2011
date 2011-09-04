#ifndef __COMM_MSGQUEUE__
#define __COMM_MSGQUEUE__

#include <stddef.h>

#include "global.h"
#include "communication/types.h"

struct MessageQueue;

union MessagePayload {
    struct {
        size_t count;
        int destinations[MAX_DESTINATIONS];
        int distances[MAX_DESTINATIONS];
    } destinations;
    struct {
        size_t count;
        int delta[MAX_STOCKS];
    } stock;
};

struct Message {
    enum MessageType type;
    union MessagePayload payload;
};

struct MessageQueue* message_queue_create(void);

void message_queue_push(struct MessageQueue* queue, struct Message msg);

struct Message message_queue_pop(struct MessageQueue* queue);

void message_queue_destroy(struct MessageQueue* queue);

#endif
