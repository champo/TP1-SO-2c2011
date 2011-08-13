#include <stdio.h>
#include "./models/city.h"
#include "./models/map.h"

Map* parseMap(const char* path){
    
    FILE* mapfile;
    Map* map;
    int i;
    City* cities;

    if ((mapfile = fopen(path, "r")) == NULL){
        return NULL;
    }

    if ((map = malloc(sizeof(Map))) == NULL){
        return NULL;    
    fscanf(mapfile, "%d\n", &(map->citycount));
    fgetc(mapfile);
    
    if ((cities = malloc(n * sizeof(City))) == NULL){
        return NULL; 
    }
    for (i = 0; i<map->citycount; i++){
        fscanf(mapfile, "%s\n", cities[i].name);
        cities[i].id = i;
        cities[i].stockSize = 0;
        while (fgetc(mapfile) != '\n'){
            cities[i].stockSize++;
            
        }
    }
    return map;

}
