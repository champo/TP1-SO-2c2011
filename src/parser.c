#include <stdio.h>
#include "models/city.h"
#include "models/airline.h"
#include "models/plane.h"
#include "utils/vector.h"
#include "parser.h"
#include "models/map.h"
#include <stdlib.h>
#include <string.h>
#include "global.h"

#define FINISHED                1
#define FIRST                   1

static void floydMatrix(int** matrix, int counter);

Map* parseMap(const char* path){

    FILE* mapfile;
    Map* ans;
    Vector* vec;
    int i,j;
    int aux, state, flag = FIRST;
    int counter;
    char buffer[CITY_NAME_MAX_LENGTH];
    char buffer2[CITY_NAME_MAX_LENGTH];
    City** cities;


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
        return NULL;
    }

    fscanf(mapfile, "%d\n\n", &counter);

    if ((cities = malloc(counter * sizeof(City*))) == NULL){
        destroyVector(ans->cities);
        free(ans);
        fclose(mapfile);
        return NULL;
    }

    for ( i=0; i<counter; i++){
        if ((cities[i] = malloc(sizeof(City))) == NULL) {
            for ( j = 0; j<i; j++) {
                free(cities[j]);
            }
            fclose(mapfile);
            free(cities);
            destroyVector(ans->cities);
            free(ans);
            return NULL;
        }
    }

    //init matrix
    if ( (ans->matrix = calloc(counter, sizeof(int *))) == NULL){
        for ( j = 0; j < counter; j++) {
            free(cities[j]);
        }
        free(cities);
        fclose(mapfile);
        destroyVector(ans->cities);
        free(ans);
        return NULL;
    }


    for (i=0; i<counter; i++){
        if( (ans->matrix[i] = calloc(counter, sizeof(int))) == NULL){
            for ( j = 0; j < i; j++) {
                free(ans->matrix[j]);
            }
            free(ans->matrix);
            for ( j = 0; j < counter; j++) {
            free(cities[j]);
            }
            free(cities);
            fclose(mapfile);
            destroyVector(ans->cities);
            free(ans);
            return NULL;
        }
    }

    if ( (ans->product = createVector()) == NULL ) {
        for ( j = 0; j < counter; j++) {
            free(ans->matrix[j]);
        }
        free(ans->matrix);
        for ( j = 0; j < counter; j++) {
            free(cities[j]);
        }
        free(cities);
        fclose(mapfile);
        destroyVector(ans->cities);
        free(ans);
        return NULL;
    }

    //Let's read each city!
    for (i = 0; i<counter; i++) {
        if (flag == FIRST) {
            if ( (cities[i]->name = malloc(CITY_NAME_MAX_LENGTH * sizeof(char))) == NULL ) {
                return NULL;
                //TODO FREE EVERYTHING
            }
            fscanf(mapfile, "%s\n", cities[i]->name);
        }
        cities[i]->id = i;
        vec = createVector();
        //Let's read the stock for the city!
        while ( ( state = fscanf(mapfile, "%s %d\n", buffer, &aux)) == 2){
            Stock* stock;
            if ((stock = initStock(buffer, aux, ans->product)) == NULL ) {
                //TODO check if this frees everything well
                return NULL;
            }
            addToVector(vec,stock);
        }
        //Let's leave everything cool for the next city...
        if (state == 1){
            flag = !FIRST;
            if ( (i+1) != counter){
                if ( (cities[i+1]->name = malloc(CITY_NAME_MAX_LENGTH * sizeof(char))) == NULL){
                    //TODO FREE EVERYTHING
                    return NULL;
                }
                strcpy(cities[i+1]->name,buffer);
                buffer[0] = 0;
            }
        }
        cities[i]->stock = vec;
        if ( addToVector(ans->cities, cities[i]) == -1 ){
            //TODO Frees1
            return NULL;
        }
    }
    //Now let's read the connections between the cities...
    if (strcmp(buffer, "") != 0) {
        fscanf(mapfile, "%s %d\n", buffer2, &aux);
        do {
            i = getCityId(buffer, ans->cities);
            j = getCityId(buffer2, ans->cities);
            ans->matrix[i][j] = aux;
            ans->matrix[j][i] = aux;
        } while (fscanf(mapfile, "%s %s %d\n", buffer, buffer2, &aux) == 3);
    }

    floydMatrix(ans->matrix, counter);

    free(cities);
    fclose(mapfile);
    return ans;
}


void floydMatrix(int** matrix, int counter) {

    int i,j,k;
    int aux1,aux2;


    for ( k=0; k<counter; k++) {
        for ( i=0; i<counter; i++) {
            for ( j=0; j<counter;j++) {
                aux2 = 0;
                aux1 = matrix[i][j];
                if (matrix[i][k] != 0 && matrix[k][j] !=0) {
                    aux2= matrix[i][k]+matrix[k][j];
                }
                if (aux2!= 0 && i != j && (aux1 == 0 || aux2 < aux1)) {
                    matrix[i][j] = aux2;
                }
            }
        }
    }
}


Airline* parseAirlines(FILE* pFile, Map* map) {

    Airline* airline = NULL;

    unsigned int i;
    int stockAmount,state,flag = FIRST;
    char cityName[CITY_NAME_MAX_LENGTH],buffer[CITY_NAME_MAX_LENGTH];
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

        if (flag == FIRST) {
            fscanf(pFile, "%s\n", cityName);
        }

        airline->planes[i].cityId = getCityId(cityName, map->cities); //TODO

        vec = createVector();
        while ((state = fscanf(pFile, "%s %d\n", buffer, &stockAmount)) == 2) {

            if ((stock = initStock(buffer, stockAmount, map->product)) == NULL) {
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
        airline->planes[i].id = i;
    }

    return airline;
}


