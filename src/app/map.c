#include "ipc/ipc.h"
#include "models/plane.h"
#include "models/city.h"
#include "models/map.h"
#include "models/airline.h"
#include "models/theShit.h"
#include "app/map.h"
#include "communication/map.h"
#include "marshall/map.h"
#include "marshall/plane.h"
#include "global.h"
#include <stdlib.h>
#include <stddef.h>

struct CityInfo {
    int cityId;
    int distance;
    int score;
};

static int endSimulation(Map* map, int turn);
static int cityIsSatisfied(City* city);
static void updateMap(Map* map, Plane* plane);
static int app_give_destinations(Map* map, Plane* plane, ipc_t conn);
static int cityInfoComparator(const void* a, const void* b);
static int insertScore(struct CityInfo* cityInfo, int size, int elems, int score);
static int getCityScore(Vector* cityStocks, Vector* planeStocks);
static void initPlane(struct StockMessagePart* stocks, struct PlaneMessageHeader* header, Plane* plane, Map* map);

void runMap(Map* map, Vector* airlines, Vector* conns){

    int i, airlinesize;
    unsigned int turn = 0;
    union PlaneMessage msg;
    Plane plane;
    int airlineId;

    airlinesize = getVectorSize(airlines);

    while (endSimulation(map, turn) == CONTINUE_SIM) {

        mprintf("Doing turn %d\n", turn++);
        comm_turn_step(conns);

        i = 0;
        while (i != airlinesize) {
            mprintf("Waiting for phase 1 action\n");
            comm_get_map_message(&msg);

            if (msg.type == MessageTypeAirlineDone) {
                i++;
                mprintf("Got %d out of %d airlines done\n", i, airlinesize);
            } else if (msg.type == MessageTypeUnloadStock) {

                airlineId = msg.stockState.header.airline;
                initPlane(&msg.stockState.stocks, &msg.stockState.header, &plane, map);
                updateMap(map, &plane);
                mprintf("Sending stocks back\n");
                comm_unloaded_stock(airlineId, &plane, (ipc_t)getFromVector(conns, airlineId));
                freeStocks(plane.stocks);
            } else {
                print_error("Got invalid message on phase 1 loop\n");
            }
        }

        mprintf("Sending continue\n");
        comm_turn_continue(conns);

        i = 0;
        while (i != airlinesize) {
            comm_get_map_message(&msg);
            if (msg.type == MessageTypeAirlineDone) {
                i++;
            } else if (msg.type == MessageTypeCheckDestinations) {
                airlineId = msg.stockState.header.airline;
                initPlane(&msg.checkDestinations.stocks, &msg.checkDestinations.header, &plane, map);
                app_give_destinations(map, &plane, (ipc_t)getFromVector(conns, airlineId));
                freeStocks(plane.stocks);
            } else {
                print_error("Got invalid message on phase 2 loop\n");
            }
        }

        //mprintf("Turn ended.. Press a key to continue...\n");
        //getchar();
        mprintf("------------------------------------------------------------------\n");
    }
    //mprintf("Done biatch.\n");
}

void initPlane(struct StockMessagePart* stocks, struct PlaneMessageHeader* header, Plane* plane, Map* map) {

    plane->id = header->id;
    if (plane->id < 0 || plane->id > 10) {
        print_error("Got invalid plane id\n");
    }

    plane->cityId = header->cityId;
    if (plane->cityId < 0 || plane->cityId > getVectorSize(map->cities)) {
        print_error("Got invalid city id\n");
    }

    plane->stocks = createVector();
    for (unsigned int j = 0; j < stocks->count; j++) {

        char name[CITY_NAME_MAX_LENGTH];
        getTheShitName(stocks->stockId[j], map->theShit, name);
        int quant = stocks->quantities[j];
        Stock* stock = initStock(name, quant, map->theShit);
        addToVector(plane->stocks, stock);

        if (quant < 0 || stocks->stockId[j] < 0) {
            print_error("Got invalid stock");
        }
    }
}


int endSimulation(Map* map, int turn) {

    size_t i;
    size_t cities = getVectorSize(map->cities);

    if (turn < 70000) {

        for (i = 0; i < cities; i++) {
            if (!cityIsSatisfied(getFromVector(map->cities, i))) {
                return CONTINUE_SIM;
            }
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


void updateMap(Map* map, Plane* plane) {

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

        if (city_stock->amount >= plane_stock->amount) {

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
        mprintf("City %d gave score %d for plane %d\n", city->id, score, plane->id);

        if (score != 0 && (index = insertScore(cityInfo, MAX_DESTINATIONS, count, score)) != -1) {
            cityInfo[index].cityId = city->id;
            cityInfo[index].distance = getDistance(map, city->id, plane->cityId);
            if (count != MAX_DESTINATIONS) {
                count++;
            }
        }
    }

    qsort(cityInfo, count, sizeof(struct CityInfo), cityInfoComparator);

    for (size_t i = 0; i < count; i++) {
        citiesIds[i] = cityInfo[i].cityId;
        distances[i] = cityInfo[i].distance;
    }

    if (count > 0) {
        mprintf("Sending to plane %d city %d (%d) with %d more\n", plane->id, citiesIds[0], distances[0], count);
    } else {
        mprintf("No options for plane %d\n", plane->id);
    }

    comm_give_destinations(plane, conn, count, citiesIds, distances);
    return 0;
}

int insertScore(struct CityInfo cityInfo[], int size, int elems, int score) {

    if (elems < size) {
        cityInfo[elems].score = score;
        return elems;
    } else {
        int min = cityInfo[0].score;
        int minIndex = 0;

        for (int i = 0; i < elems; i++) {
            if ( cityInfo[i].score < min) {
                min = cityInfo[i].score;
                minIndex = i;
            }
        }
        if (score > cityInfo[minIndex].score) {
            cityInfo[minIndex].score = score;
            return minIndex;
        } else {
            return -1; // Nothing was inserted
        }
    }
}

int cityInfoComparator(const void* a, const void* b) {
    return (((const struct CityInfo*)a)->distance - ((const struct CityInfo*)b)->distance);
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

