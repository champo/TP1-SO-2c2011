#include <stdio.h>
#include "models/city.h"
#include "models/airline.h"
#include "models/plane.h"
#include "utils/vector.h"
#include <stdlib.h>
#include <string.h>

#define FINISHED                1
#define NAME_MAX_LENGTH         30
#define FIRST                   1

Airline* parseAirlines(FILE* pFile); 

Stock* initStock(); 

//TODO SON SOLO DEFS DSP HAY Q IMPLEMENTARLAS Y  MOVERLAS!!!!
int getCityId(char* cityName) {
    return strlen(cityName);
}

//TODO IDEM
int getTheShitId(char* theShitName) {
    return strlen(theShitName);
}


Vector* parseMap(const char* path){
    
    FILE* mapfile;
    Vector* map;
    int i;
    int counter;
    City* cities;

    if((map = create()) == NULL){
        return NULL;
    }

    if ((mapfile = fopen(path, "r")) == NULL){
        return NULL;
    }

    fscanf(mapfile, "%d\n", &counter);
    fgetc(mapfile);
    
    if ((cities = malloc(n * sizeof(City))) == NULL){
        return NULL; 
    }
    for (i = 0; i<counter; i++){
        fscanf(mapfile, "%s\n", cities[i].name);
        cities[i].id = i;
        cities[i].stockSize = 0;
        while (fgetc(mapfile) != '\n'){
            cities[i].stockSize++;
            
        }
    }
    return map;
}



Airline* parseAirlines(FILE* pFile) {
    Airline* airline;
    unsigned int i;
    int aux,state,flag = FIRST;
    char cityName[NAME_MAX_LENGTH] ;          
    char buffer[NAME_MAX_LENGTH];
    Vector* vec; 
    
    if ( (airline = malloc(sizeof(Airline))) == NULL ) {
        return NULL;
    }
    
    fscanf(pFile,"%u",&(airline->numberOfPlanes));

    if ( (airline->planes = calloc (airline->numberOfPlanes,sizeof(Plane))) == NULL) {
        return NULL;
    }
    for (i = 0; i < airline->numberOfPlanes; i++) {
        if(flag == FIRST) {
            fscanf(pFile,"%s\n",cityName); 
        }
        airline->planes[i].cityId = getCityId(cityName); //TODO
        
        vec = create();
        while ( ( state = fscanf(pFile,"%s %d\n",buffer,&aux)) == 2 ) { 
           
            Stock* stock = initStock(); 
            if ( (stock->theShit->name = malloc(strlen(buffer)*sizeof(char))) == NULL ) {
                return NULL;
            }
            strcpy(stock->theShit->name,buffer);
            stock->amount = aux;
            stock->theShit->id = getTheShitId(stock->theShit->name);  //TODO
            add(vec,stock);
        }
        if (state == 1) {
            flag = !FIRST;
            strcpy(cityName,buffer);
        }
        airline->planes[i].stocks = vec;
        //TODO ASIGN PLANE ID
    } 
    return airline;
} 


Stock* initStock() {

    Stock* stock;
    TheShit* theShit;
    if ( (stock = malloc(sizeof(Stock))) == NULL ) {
        return NULL;
    }
    if ( (theShit = malloc(sizeof(TheShit))) == NULL ) {
        return NULL;
    }
    stock->theShit = theShit;
    return stock;
}

