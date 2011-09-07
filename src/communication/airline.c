#include "communication/airline.h"

#include <string.h>

#include "ipc/ipc.h"
#include "marshall/map.h"

int comm_airline_recieve(union MapMessage* msg) {
    return ipc_read(msg, sizeof(union MapMessage)) == -1 ? -1 : 0;
}

int comm_airline_ready(ipc_t conn) {
    enum MessageType type = MessageTypeAirlineDone;
    return ipc_write(conn, &type, sizeof(type)) == -1 ? -1 : 0;
}

