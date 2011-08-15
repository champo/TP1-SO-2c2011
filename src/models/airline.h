#ifndef __AIRLINE__
#define __AIRLINE__

#include <stddef.h>
#include "models/plane.h"

typedef struct {
    int     id;
    size_t  numberOfPlanes;      
    Plane*  planes;
} Airline;

#endif
