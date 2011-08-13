#ifndef __CITY__
#define __CITY__
#include <stddef.h>
#include "./stock.h"

typedef struct {
    char*   name;
    int     id;
    Stock*  stock;
    size_t  stockSize;

} City;

#endif
