#include "ipc/ipc.h"
#include "models/plane.h"
#include "models/city.h"
#include "models/map.h"
#include "models/airline.h"
#include "app/map.h"
#include "communication/map.h"
#include "marshall/map.h"
#include "marshall/plane.h"
#include "global.h"
#include <stdlib.h>
#include <stddef.h>

static struct CityInfo {
    int cityId;
    int distance;
    int score;
};

static struct MapMessage getMessageForMap(void);

static int endSimulation(Map* map);
static int cityIsSatisfied(City* city);
static void updateMap(Map* map, Plane* plane);
static void turnStep(Vector* conns);
static void turnContinue(Vector* conns);
static int app_give_destinations(Map* map, Plane* plane, ipc_t conn);
static int cityInfoComparator(const void* a, const void* b);
static int insertScore(struct CityInfo* cityInfo, int size, int elems, int score);
static int getCityScore(Vector* cityStocks, Vector* planeStocks);

void runMap(Map* map, Vector* airlines, Vector* conns){

    int i,airlinesize;
    struct MapMessage msg;

    airlinesize = getVectorSize(airlines);
    i = 0;

    while (endSimulation(map)) {

            turnStep(conns);

            while (i != airlinesize) {
                msg = getMessageForMap();

                if (msg.type == MessageTypeAirlineDone) {
                    i++;
                }
                if (msg.type == MessageTypeUnloadStock) {
                    updateMap(map, &(msg.planeInfo.plane));
                    comm_unloaded_stock(msg.planeInfo.airlineID , &(msg.planeInfo.plane), (ipc_t)getFromVector(conns,msg.planeInfo.airlineID));
                }
            }

            i = 0;

            turnContinue(conns);

            while (i != airlinesize) {
                msg = getMessageForMap();
                if (msg.type == MessageTypeAirlineDone) {
                    i++;
                }
                if (msg.type == MessageTypeCheckDestinations) {
                    app_give_destinations(map, &(msg.planeInfo.plane), (ipc_t)getFromVector(conns,msg.planeInfo.airlineID));
                }
            }



    }
}

struct MapMessage getMessageForMap(void){
    struct MapMessage msg;
    return msg;
}

int endSimulation(Map* map){

    size_t i;
    size_t cities = getVectorSize(map->cities);
    for (i = 0; i < cities; i++) {
        if ( !cityIsSatisfied(getFromVector(map->cities, i))) {
            return CONTINUE_SIM;
        }
    }
    return END_SIM;
}

int cityIsSatisfied(City* city) {

    size_t i;
    size_t stock_size = getVectorSize(city->stock);
    for (i = 0; i < stock_size; i++) {
        Stock* stock = getFromVector(city->stock, i);
        if (stock->amount != 0) {
            return 0;
        }
    }
    return 1;
}


void updateMap(Map* map, Plane* plane){

    City* city = getFromVector(map->cities, plane->cityId);
    size_t i;
    size_t plane_stock_size = getVectorSize(plane->stocks);

    for (i = 0; i < plane_stock_size; i++) {

        Stock* plane_stock = getFromVector(plane->stocks,i);
        Stock* city_stock;
        size_t j;
        size_t city_stock_size = getVectorSize(city->stock);
        for(j = 0; j < city_stock_size; j++) {

            city_stock = getFromVector(city->stock,j);
            if ( plane_stock->theShit->id == city_stock->theShit->id) {
                break;
            }
        }

        if (city_stock->amount >= plane_stock->amount ) {

            // Then discharge everything
            city_stock->amount -= plane_stock->amount;
            plane_stock->amount = 0;
        } else {

            // Satisfy all the city needs
            plane_stock->amount -= city_stock->amount;
            city_stock->amount = 0;
        }
    }

    return;
}

int app_give_destinations(Map* map, Plane* plane, ipc_t conn) {

    struct CityInfo cityInfo[MAX_DESTINATIONS];
    int citiesIds[MAX_DESTINATIONS];
    int distances[MAX_DESTINATIONS];
    size_t count = 0, score;
    size_t cityNumber = getVectorSize(map->cities);
    int index;

    for (size_t i = 0; i < cityNumber; i++) {

        City* city = getFromVector(map->cities, i);
        score = getCityScore(city->stock, plane->stocks);
        if (score != 0 && (index = insertScore(cityInfo, MAX_DESTINATIONS, count, score) != -1)) {
            cityInfo[index].cityId = city->id;
            cityInfo[index].distance = getDistance(map, city->id, plane->cityId);
            count++;
        }
    }

    qsort(cityInfo, count, sizeof(struct CityInfo), cityInfoComparator);

    for (size_t i = 0; i < count; i++) {
        citiesIds[i] = cityInfo[i].cityId;
        distances[i] = cityInfo[i].distance;
    }

    comm_give_destinations(plane, conn, count, citiesIds, distances);

    return 0;
}

int insertScore(struct CityInfo* cityInfo, int size, int elems, int score) {


    if( elems < size ) {
        cityInfo[elems].score = score;
    } else {
        int min = cityInfo[0].score;
        int minIndex = 0;

        for (int i = 0; i < size; i++) {
            if ( cityInfo[i].score < min) {
                min = cityInfo[i].score;
                minIndex = i;
            }
        }
        if (score > cityInfo[minIndex].score) {
            cityInfo[minIndex].score = score;
        } else {

            return -1; // Nothing was inserted
        }
    }
    return score;
}

int cityInfoComparator(const void* a, const void* b) {
    return (((const struct CityInfo*)a)->score - ((const struct CityInfo*)b)->score);
}


void turnStep(Vector* conns) {
    comm_turn_step(conns);
    return;
}

int getCityScore(Vector* cityStocks, Vector* planeStocks) {

    size_t cityStockSize = getVectorSize(cityStocks);
    size_t planeStockSize = getVectorSize(planeStocks);
    int score = 0;

    for (size_t i = 0; i < cityStockSize; i++) {
        Stock* cityStock = getFromVector(cityStocks, i);
        for (size_t j = 0; j < planeStockSize; j++) {
            Stock* planeStock = getFromVector(planeStocks, j);
            if (cityStock->theShit->id ==  planeStock->theShit->id) {
                score += (cityStock->amount > planeStock->amount)? planeStock->amount : cityStock->amount;
                break;
            }
        }
    }
    return score;
}


void turnContinue(Vector* conns) {
    comm_turn_continue(conns);
    return;
}

