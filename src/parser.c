#include <stdio.h>
#include "models/city.h"
#include "models/airline.h"
#include "models/plane.h"
#include "utils/vector.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>

#define FINISHED                1
#define NAME_MAX_LENGTH         30
#define FIRST                   1



//Muchas funcs, como las de free o init  deberian ir a otro arch?
Stock* initStock(); 
int getCityId(char* cityName);
int getTheShitId(char* theShitName); 
void freeTheShit(TheShit* theShit);
void freeStock(Vector* stock);
void freePlane(Plane* plane);
void freeAirline(Airline* airline);


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
    Airline* airline = NULL;
    unsigned int i;
    int aux,state,flag = FIRST;
    char cityName[NAME_MAX_LENGTH] ;          
    char buffer[NAME_MAX_LENGTH];
    Vector* vec = NULL; 
    
    if ( (airline = malloc(sizeof(Airline))) == NULL ) {
        return NULL;
    }
    
    fscanf(pFile,"%u",&(airline->numberOfPlanes));

    if ( (airline->planes = calloc (airline->numberOfPlanes,sizeof(Plane))) == NULL) {
        freeAirline(airline);
        return NULL;
    }
    for ( i = 0; i < airline->numberOfPlanes; i++) {
        if(flag == FIRST) {
            fscanf(pFile,"%s\n",cityName); 
        }
        airline->planes[i].cityId = getCityId(cityName); //TODO
        
        vec = createVector();
        while ( ( state = fscanf(pFile,"%s %d\n",buffer,&aux)) == 2 ) { 
           
            Stock* stock = initStock(); 
            if ( (stock->theShit->name = malloc(strlen(buffer)*sizeof(char))) == NULL ) {
                freeAirline(airline);
                return NULL;
            }
            strcpy(stock->theShit->name,buffer);
            stock->amount = aux;
            stock->theShit->id = getTheShitId(stock->theShit->name);  //TODO
            addToVector(vec,stock);
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





void freeAirline(Airline* airline) {
    unsigned int i;
    if ( airline != NULL) {
        for ( i = 0; i < airline->numberOfPlanes; i++) {
            freePlane(&airline->planes[i]);
        }
        free(airline);
    }
}



void freePlane(Plane* plane) {
    if ( plane != NULL) {
        freeStock(plane->stocks);
        free(plane);
    }
}


void freeStock(Vector* stocks) {
    unsigned int i;
    if ( stocks != NULL) {
        for ( i = 0; i < getVectorSize(stocks); i++) {
            freeTheShit(    ((Stock*) getFromVector(stocks,i))->theShit);
        }
        destroyVector(stocks);
    }
}

void freeTheShit(TheShit* theShit) {
    if ( theShit != NULL) {
        free(theShit->name);
        free(theShit);
    }
}
