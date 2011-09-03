#ifndef __COMM_MAP__
#define __COMM_MAP__


int comm_unloaded_stock(int airlineID, Plane* plane, ipc_t conn);

int comm_give_destinations(Plane* plane, ipc_t conn, int cityNumber, City* cities, int* distances);
#endif
