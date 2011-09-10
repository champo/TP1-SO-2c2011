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
    MessageTypeLast // Do not use and keep always in the end, it's for debugging
};

#endif
