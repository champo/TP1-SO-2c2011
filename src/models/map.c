#include "utils/vector.h"
#include "models/theShit.h"
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

    counter = getVectorSize(map->theShit);

    for ( i=0; i<counter; i++){
        freeTheShit((TheShit*)getFromVector(map->theShit,i));
    }

    destroyVector(map->cities);
    destroyVector(map->theShit);
    free(map);
    return;
}

int getDistance(Map* map, int city1, int city2) {

    return map->matrix[city1][city2];
}
