#ifndef __OUTPUT__
#define __OUTPUT__

#include "communication/msgqueue.h"
#include "utils/sem.h"

void run_output(struct MessageQueue* outputMsgQueue, semv_t sem);

#endif
