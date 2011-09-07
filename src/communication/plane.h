#ifndef __COMM_PLANE__
#define __COMM_PLANE__

#include "app/plane.h"

#define transmit(conn, msg) if (sizeof(msg) != ipc_write(conn, &(msg), sizeof(msg))) { \
        return -1; \
        }

int comm_check_destinations(struct PlaneThread* plane, int* destinations, int* distances, size_t* len);

int comm_continue(struct PlaneThread* plane);

int comm_step(struct PlaneThread* plane);

int comm_unload_stock(struct PlaneThread* plane, int* stockDelta);

#endif
