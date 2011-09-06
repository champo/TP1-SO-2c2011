#ifndef __COMM_TYPES__
#define __COMM_TYPES__

enum MessageType {
    MessageTypeNone = 0,
    MessageTypeContinue,
    MessageTypeDestinations,
    MessageTypeStep,
    MessageTypeStock,
    MessageTypeAirlineDone,
    MessageTypeInTransit,
    MessageTypeUnloadStock,
    MessageTypeCheckDestinations
};

#endif
