#include "communication/airline.h"

#include <string.h>

#include "ipc/ipc.h"
#include "marshall/map.h"

int comm_airline_recieve(struct MapMessage* msg) {
    char buff[IPC_MAX_PACKET_LEN];
    int len;
    if ((len = ipc_read(buff, IPC_MAX_PACKET_LEN)) == -1) {
        return -1;
    }

    memcpy(msg, buff, len);
    return 0;
}

int comm_airline_ready(ipc_t conn) {
    enum MessageType type = MessageTypeAirlineDone;
    return ipc_write(conn, &type, sizeof(type)) == -1 ? -1 : 0;
}

