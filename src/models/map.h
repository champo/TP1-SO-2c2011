#ifndef __MAP__
#define __MAP__

#include "utils/vector.h"
#include "models/city.h"

typedef struct {
    Vector* cities;
    Vector* theShit;
    int** matrix;
} Map;

void freeMap(Map* map);
int getDistance(Map* map, int city1, int city2);

#endif
