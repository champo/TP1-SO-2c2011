#ifndef __APP_AIRLINE__
#define __APP_AIRLINE__

#include "models/airline.h"
#include "ipc/ipc.h"

void run_airline(Airline* self, ipc_t conn);

#endif
