#ifndef __MODELS_AIRLINE__
#define __MODELS_AIRLINE__

#include <stddef.h>
#include "models/plane.h"


typedef struct {
    int     id;
    size_t  numberOfPlanes;      
    Plane*  planes;
} Airline;

void freeAirline(Airline* airline);


#endif


