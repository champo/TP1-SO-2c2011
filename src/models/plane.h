#ifndef __PLANE__
#define __PLANE__
#include<types.h>

typedef struct {
    int     id;
    size_t  stockSize;
    Stock   stock[];   
} Plane;

#endif
