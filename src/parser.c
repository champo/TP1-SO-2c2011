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




int getCityId(char* cityName);
int getTheShitId(char* theShitName); 



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

    fscanf(mapfile, "%d", &counter);
    
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
            fscanf(mapfile, "%s\n", cities[i].name);
        }
        cities[i].id = i;
        vec = createVector();
        while ( ( state = fscanf(mapfile, "%s %d\n", buffer, aux)) == 2){			
			char* name;
			if ( (name = malloc(strlen(buffer)*sizeof(char))) == NULL ) {
				//TODO check if this frees everything well
				return NULL;
			}
			strcpy(name,buffer);
			Stock* stock;
			if ((stock = initStock(name,aux)) == NULL ) {
				//TODO check if this frees everything well
				return NULL;
			}
			addToVector(vec,stock);
        }
        if (state == 1){
            flag = !FIRST;
            if ( (i+1) != counter){
                strcpy(cities[i+1].name,buffer);
                buffer[0] = "\0";
            } 
        }
        cities[i].stock = vec;
        if ( addToVector(ans->vec, &(cities[i])) == -1 ){
            //TODO Frees
            return NULL;
        }
    }

    
    if (strcmp(buffer, "") == 0){
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
			char* name;
			if ( (name = malloc(strlen(buffer)*sizeof(char))) == NULL ) {
				freeAirline(airline); //TODO check if this frees everything well
				return NULL;
			}
			strcpy(name,buffer);
			Stock* stock;
			if ((stock = initStock(name,aux)) == NULL ) {
				freeAirline(airline); //TODO check if this frees everything well
				return NULL;
			}
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


extern int testAirlineParser(void);

int main(void) {
   testAirlineParser();

}
