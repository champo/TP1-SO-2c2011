#ifndef __UTILS_MSGQUEUE__
#define __UTILS_MSGQUEUE__

struct MessageQueue;

union MessagePaylod;

enum MessageType;

struct Message {
    enum MessageType type;
    union MessagePayload payload;
};

struct MessageQueue* message_queue_create(void);

void message_queue_push(struct MessageQueue* queue, struct Message* msg);

struct Message* message_queue_pop(struct MessageQueue* queue);

void message_queue_destroy(struct MessageQueue* queue);

#endif
