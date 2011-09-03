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




int getCityId(char* cityName, Vector* cities);
int getTheShitId(char* theShitName, Vector* theShit); 
static void floydMatrix(int** matrix, int counter);


//TODO SON SOLO DEFS DSP HAY Q IMPLEMENTARLAS Y  MOVERLAS!!!!
int getCityId(char* cityName, Vector* cities) {
    int i;

    for ( i = 0; i<getVectorSize(cities); i++ ){
        if ( strcmp(cityName, ((City*)getFromVector(cities,i))->name) == 0 ){
            return ((City*)getFromVector(cities,i))->id;
        }
    }
    return -1;
}

//TODO IDEM
int getTheShitId(char* theShitName, Vector* theShit) {
    int i;

    for (i =0; i<getVectorSize(theShit); i++){
        if ( strcmp(theShitName, ((TheShit*)getFromVector(theShit,i))->name) ==0){
            return ((TheShit*)getFromVector(theShit,i))->id;
        }
    }
    return -1;
}


Map* parseMap(const char* path){
    
    FILE* mapfile;
    Map* ans;
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


    if ((mapfile = fopen(path, "r")) == NULL){
        free(ans);
        return NULL;
    }


    if( (ans->cities = createVector()) == NULL){
        free(ans);
        fclose(mapfile);
    }
    if ((cities = malloc(counter * sizeof(City))) == NULL){
        destroyVector(ans->cities);
        free(ans);
        fclose(mapfile);
        return NULL; 
    }
    fscanf(mapfile, "%d\n\n", &counter);
    //init matrix
    if ( (ans->matrix = malloc(counter * sizeof(int *))) == NULL){
        free(ans);
        fclose(mapfile);
        destroyVector(ans->cities);
        free(cities);
        return NULL;
    }
    
    for (i=0; i<counter; i++){
        if( (ans->matrix[i] = calloc(counter, sizeof(int))) == NULL){
            //TODO hacer frees
            return NULL;
        }
    }

    if ( (ans->theShit = createVector()) == NULL ){
        //TODO FALTAN FREES
        return NULL;
    }
   
    //Let's read each city!   
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
        //Let's read the stock for the city!
        while ( ( state = fscanf(mapfile, "%s %d\n", buffer, &aux)) == 2){			
			char* name;
			if ( (name = malloc(strlen(buffer)*sizeof(char))) == NULL ) {
				//TODO check if this frees everything well
				return NULL;
			}
			strcpy(name,buffer);
			Stock* stock;
			if ((stock = initStock(name,aux, ans->theShit)) == NULL ) {
				//TODO check if this frees everything well
				return NULL;
			}
			addToVector(vec,stock);
        }
        //Let's leave everything cool for the next city...
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
        if ( addToVector(ans->cities, &(cities[i])) == -1 ){
            //TODO Frees1
            return NULL;
        }
    }
    //Now let's read the connections between the cities...
    if (strcmp(buffer, "") != 0){
        fscanf(mapfile, "%s %d\n", buffer2, &aux);
        ans->matrix[getCityId(buffer, ans->cities)][getCityId(buffer2, ans->cities)] = aux;
        ans->matrix[getCityId(buffer2, ans->cities)][getCityId(buffer, ans->cities)] = aux;
        while ( fscanf(mapfile, "%s %s %d\n", buffer, buffer2, &aux) == 3){
            ans->matrix[getCityId(buffer, ans->cities)][getCityId(buffer2, ans->cities)] = aux;
            ans->matrix[getCityId(buffer2, ans->cities)][getCityId(buffer, ans->cities)] = aux;
        }
        
    }

    floydMatrix(ans->matrix, counter);

    return ans;
}


void floydMatrix(int** matrix, int counter) {
    
    int i,j,k;
    int aux1,aux2;
        
    for ( k=0; k<counter; k++) {
        for ( i=0; i<counter; i++) {
            for ( i=0; i<counter;i++) {
                aux1 = matrix[i][j];
                aux2= matrix[i][k]+matrix[k][j];
                matrix[i][j] = aux1 < aux2 ? aux1 : aux2;
            }
        }
    }
}


Airline* parseAirlines(FILE* pFile, Map* map) {
    Airline* airline = NULL;
    unsigned int i;
    int stockAmount,state,flag = FIRST;
    char cityName[NAME_MAX_LENGTH],buffer[NAME_MAX_LENGTH];
    Vector* vec = NULL; 
	Stock* stock;
    
    if ((airline = calloc(1, sizeof(Airline))) == NULL) {
        return NULL;
    }
    
    fscanf(pFile, "%u", &(airline->numberOfPlanes));

    if ((airline->planes = calloc(airline->numberOfPlanes, sizeof(Plane))) == NULL) {
        freeAirline(airline);
        return NULL;
    }
    for (i = 0; i < airline->numberOfPlanes; i++) {

        if(flag == FIRST) {
            fscanf(pFile, "%s\n", cityName); 
        }
        
        airline->planes[i].cityId = getCityId(cityName, map->cities); //TODO
        
        vec = createVector();
        while ((state = fscanf(pFile, "%s %d\n", buffer, &stockAmount)) == 2) { 
			
            char* theShitName;
			if ((theShitName = malloc(strlen(buffer) * sizeof(char))) == NULL) {
				freeAirline(airline); //TODO check if this frees everything well
				return NULL;
			}
			strcpy(theShitName,buffer);
			
            if ((stock = initStock(theShitName, stockAmount, map->theShit)) == NULL) {
				freeAirline(airline); //TODO check if this frees1 everything well
				return NULL;
			}
			addToVector(vec, stock);
        }
        if (state == 1) {
            flag = !FIRST;
            strcpy(cityName, buffer);
        }

        airline->planes[i].stocks = vec;
        //TODO ASIGN PLANE ID
    } 
    return airline;
} 


