#ifndef __CITY__
#define __CITY__

#include "utils/vector.h"

typedef struct {
    char*   name;
    int     id;
    Vector* stock;
} City;

#endif
