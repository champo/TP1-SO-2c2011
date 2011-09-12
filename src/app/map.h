#ifndef __APP__MAP__
#define __APP__MAP__

#define CONTINUE_SIM    1
#define END_SIM         0

#include <pthread.h>
#include "utils/vector.h"
#include "utils/sem.h"
#include "models/map.h"
#include "communication/msgqueue.h"

void runMap(Map* map, Vector* airlines, Vector* conns, int* exitState, struct MessageQueue* outputMessageQueue, semv_t outputSem);

#endif
