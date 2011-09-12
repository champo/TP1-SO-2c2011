#include "utils/vector.h"
#include "models/product.h"
#include "models/city.h"
#include "models/map.h"

#include <stdlib.h>

void freeMap(Map* map){
    int i;
    int counter = getVectorSize(map->cities);

    for ( i=0; i<counter; i++){
        freeCity((City*)getFromVector(map->cities,i));
        free((map->matrix)[i]);
    }
    free(map->matrix);

    counter = getVectorSize(map->product);

    for ( i=0; i<counter; i++){
        freeProduct((Product*)getFromVector(map->product,i));
    }

    destroyVector(map->cities);
    destroyVector(map->product);
    free(map);
    return;
}

int getDistance(Map* map, int city1, int city2) {

    return map->matrix[city1][city2];
}
