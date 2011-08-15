#ifndef __PLANE__
#define __PLANE__


#include "models/stock.h"
#include "utils/vector.h"

typedef struct {
    int     id;
    Vector* stocks;
    int     cityId;
} Plane;

#endif
