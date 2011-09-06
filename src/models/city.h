#ifndef __CITY__
#define __CITY__

#include "utils/vector.h"

typedef struct {
    char*   name;
    int     id;
    Vector* stock;
} City;

int getCityId(const char* cityName, Vector* cities);

void freeCity(City* city);

#endif
