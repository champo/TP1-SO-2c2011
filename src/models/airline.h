#ifndef __AIRLINE__
#define __AIRLINE__

#include<types.h>

typedef struct {
    int     id;
    size_t  numberOfPlanes;      
    Plane*  planes;
} Airline;

#endif
