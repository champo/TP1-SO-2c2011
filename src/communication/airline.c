#include "communication/airline.h"

#include <string.h>

#include "ipc/ipc.h"
#include "marshall/map.h"
#include "communication/map.h"

int comm_airline_recieve(union MapMessage* msg) {
    return ipc_read(msg, sizeof(union MapMessage)) == -1 ? -1 : 0;
}

int comm_airline_ready(ipc_t conn) {
    enum MessageType type = MessageTypeAirlineDone;
    mprintf("Sending ready...\n");
    return ipc_write(conn, &type, sizeof(type)) == -1 ? -1 : 0;
}

int comm_airline_status(ipc_t conn, int id, int flying, int total) {

    struct AirlineStatusMessage msg;
    msg.type = MessageTypeAirlineStatus;
    msg.status = marshall_send_airline_status(flying, total, id);

    return ipc_write(conn, &msg, sizeof(msg)) == -1 ? -1 : 0;
}

