#ifndef __STOCK__
#define __STOCK__

#include "models/theShit.h"
#include "utils/vector.h"



typedef struct {
    int          amount;
    TheShit*     theShit;
} Stock;

Stock* initStock(char* name, int amount); 
void freeStock(Vector* stocks);
#endif
