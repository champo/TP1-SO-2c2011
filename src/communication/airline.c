#include "communication/airline.h"

#include "ipc/ipc.h"

int comm_airline_recieve(struct MapMessage* msg) {
    char buff[IPC_MAX_PACKET_LEN];
    int len;
    if ((len = ipc_read(buff, IPC_MAX_PACKET_LEN)) == -1) {
        return -1;
    }

    memcpy(msg, buff, len);
    return 0;
}

