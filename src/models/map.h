#ifndef __MAP__
#define __MAP__

#include "utils/vector.h"

typedef struct {
    Vector* cities;
    Vector* theShit;
    int** matrix;
} Map;

void freeMap(Map* map);

#endif
