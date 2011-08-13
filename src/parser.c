#include <stdio.h>
#include "./models/city.h"
#include "./models/airline.h"
#include "./models/plane.h"
#include <stdlib.h>

/*
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
*/


Airline* parseAirlines() {

    Airline* airline;
    if ( (airline= malloc(sizeof(Airline))) == NULL ) {
        return NULL;
    }
    FILE * pFile;
    if ( (pFile = fopen ("empresa.txt","r")) == NULL) {
        return NULL;
    }
    fscanf(pFile,"%u\n",&(airline->numberOfPlanes));
    fscanf(pFile,"\n");
   
    Plane* planes;
    if ( (planes = calloc (airline->numberOfPlanes,sizeof(Plane))) == NULL) {
        return NULL;
    }

}
