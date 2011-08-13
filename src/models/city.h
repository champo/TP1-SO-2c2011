#ifndef __CITY__
#define __CITY__
#include <stddef.h>
#include "./utils/vector.h"

typedef struct {
    char*   name;
    int     id;
    Vector* stock;
} City;

#endif
