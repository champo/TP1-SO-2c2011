#ifndef __COMM_TYPES__
#define __COMM_TYPES__

enum MessageType {
    MessageTypeNone = 0,
    MessageTypeContinue,
    MessageTypeDestinations,
    MessageTypeStep,
    MessageTypeStock,
    MessageTypeAirlineDone,
    MessageTypeUnloadStock,
    MessageTypeCheckDestinations,
    MessageTypeEnd,
    MessageTypeMapStatus,
    MessageTypeAirlineStatus,
    MessageTypeEndOutput,
    MessageTypeLast // Do not use and keep always in the end, it's for debugging
};

struct AirlineStatus {
    int id;
    int planesFlying;
    int totalPlanes;
};

struct AirlineStatusMessage {
    enum MessageType type;
    struct AirlineStatus status;
};

#endif
