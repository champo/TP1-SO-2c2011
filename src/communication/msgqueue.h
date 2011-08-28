#ifndef __COMM_MSGQUEUE__
#define __COMM_MSGQUEUE__

struct MessageQueue;

union MessagePaylod {
};

enum MessageType {
    MessageTypeNone = 0
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
