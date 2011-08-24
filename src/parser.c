#include <stdio.h>
#include "models/city.h"
#include "models/airline.h"
#include "models/plane.h"
#include "utils/vector.h"
#include "parser.h"
#include "models/map.h"
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


Map* parseMap(const char* path){
    
    FILE* mapfile;
    Map* ans;
    Vector* map;
    Vector* vec;
    int i;
    int aux, state, flag = FIRST;
    int counter;
    char buffer[NAME_MAX_LENGTH];
    char buffer2[NAME_MAX_LENGTH];
    City* cities;
    

    if ( (ans = malloc(sizeof(Map))) == NULL){
        return NULL;
    }


    if ((map = createVector()) == NULL){
        free(ans);
        return NULL;
    }


    if ((mapfile = fopen(path, "r")) == NULL){
        free(ans);
        destroyVector(map);
        return NULL;
    }


    if( (ans->vec = createVector()) == NULL){
        free(ans);
        destroyVector(map);
        fclose(mapfile);
    }


    fscanf(mapfile, "%d\n\n", &counter);
    
    

    //init matrix
    if ( (ans->matrix = malloc(counter * sizeof(int *))) == NULL){
        //TODO hacer frees
        return NULL;
    }
    for (i=0; i<counter; i++){
        if( (ans->matrix[i] = calloc(counter, sizeof(int))) == NULL){
            //TODO hacer frees
            return NULL;
        }
    }


    if ((cities = malloc(counter * sizeof(City))) == NULL){
        destroyVector(ans->vec);
        free(ans);
        destroyVector(map);
        fclose(mapfile);
        return NULL; 
    }
    

    for (i = 0; i<counter; i++){
        if (flag == FIRST){
            if ( (cities[i].name = malloc(NAME_MAX_LENGTH * sizeof(char))) == NULL ){
                return NULL;
                //TODO FREE EVERYTHING
            }
            fscanf(mapfile, "%s\n", cities[i].name);
        }
        cities[i].id = i;
        vec = createVector();
        while ( ( state = fscanf(mapfile, "%s %d\n", buffer, &aux)) == 2){
            Stock* stock = initStock();

            if ( (stock->theShit->name = malloc(strlen(buffer)*sizeof(char))) == NULL ){
                //TODO FREE EVERYTHING
                return NULL;
            }
            strcpy(stock->theShit->name,buffer);
            stock->amount = aux;
            stock->theShit->id = getTheShitId(stock->theShit->name);
            addToVector(vec,stock);
        }
        printf("finished stock: %d\n",i);
        if (state == 1){
            flag = !FIRST;
            if ( (i+1) != counter){
                if ( (cities[i+1].name = malloc(NAME_MAX_LENGTH * sizeof(char))) == NULL){
                    //TODO FREE EVERYTHING
                    return NULL;
                }
                strcpy(cities[i+1].name,buffer);
                buffer[0] = "\0";
            } 
        }
        cities[i].stock = vec;
        if ( addToVector(ans->vec, &(cities[i])) == -1 ){
            //TODO Frees1
            return NULL;
        }
    }

   
    if (strcmp(buffer, "") != 0){
        fscanf(mapfile, "%s %d", buffer2, &aux);
        ans->matrix[getCityId(buffer)][getCityId(buffer2)] = aux;
        ans->matrix[getCityId(buffer2)][getCityId(buffer)] = aux;
        while ( fscanf(mapfile, "%s %s %d", buffer, buffer2, &aux) == 2){
            ans->matrix[getCityId(buffer)][getCityId(buffer2)] = aux;
            ans->matrix[getCityId(buffer2)][getCityId(buffer)] = aux;
        }
        
    }

    return ans;
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
