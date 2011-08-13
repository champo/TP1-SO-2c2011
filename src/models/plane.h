#ifndef __PLANE__
#define __PLANE__
#include <stddef.h>
#include "./models/stock.h"

typedef struct {
    int     id;
    size_t  stockSize;
    Stock*   stock;   
    int     cityId;
} Plane;

#endif
