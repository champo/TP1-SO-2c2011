#include <stdio.h>
#include "./models/city.h"
#include "./models/airline.h"
#include "./models/plane.h"
#include <stdlib.h>
#include "./utils/vector.h"


Vector* parseMap(const char* path){
    
    FILE* mapfile;
    Vector* map;
    int i;
    int counter;
    City* cities;

    if((map = createVector()) == NULL){
        return NULL;
    }

    if ((mapfile = fopen(path, "r")) == NULL){
        return NULL;
    }

    fscanf(mapfile, "%d\n", &counter);
    fgetc(mapfile);
    
    if ((cities = malloc(counter * sizeof(City))) == NULL){
        return NULL; 
    }
    for (i = 0; i<counter; i++){
        fscanf(mapfile, "%s\n", cities[i].name);
        cities[i].id = i;
        while (fgetc(mapfile) != '\n'){
            
        }
    }
    return map;
}


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
