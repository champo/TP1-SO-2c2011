#include <stdlib.h>
#include "ipc/ipc.h"
#include "models/plane.h"
#include "models/city.h"
#include "models/map.h"
#include "models/airline.h"
#include "app/map.h"
#include "communication/map.h"

static int getMessageForMap(Plane* plane, int* airlineID);
static int endSimulation(Map* map);
static int cityIsSatisfied(City* city);
static void updateMap(Map* map, Plane* plane);
static void giveDirections(Map* map, Plane plane, ipc_t conn);
static void startPhaseTwo(Vector* conns);

void runMap(Map* map, Vector* airlines, Vector* conns){
    
    int i,temp,airlinesize,airlineID;
    Plane curplane;

    airlinesize = getVectorSize(airlines);
    i = 0;
    
    while (endSimulation(map)) {
            while (i != airlinesize) {
                temp = getMessageForMap(&curplane, &airlineID); 
                                                   //returns 0 if it has read a plane which wants to discharge,
                                                   // 1 if it has read an end airline message
                                                   // -1 any other choice
                
                if (temp == 1) {
                    i++;
                }
                if (temp == 0) {
                    updateMap(map, &curplane);
                    comm_unloaded_stock(&curplane, (ipc_t)getFromVector(conns,airlineID));
                }
            }

            i = 0;
            
            startPhaseTwo(conns);

            while (i != airlinesize) {
                temp = getMessageForMap(&curplane, &airlineID); 
                                                  //returns 0 if it has read a plane which wants instructions,
                                                   // 1 if it has read an end airline message
                                                   // -1 any other choice
                
                if (temp == 1) {
                    i++;
                }
                if (temp == 0) {
                    giveDirections(map, curplane, (ipc_t)getFromVector(conns,airlineID));
                }
            }
            

            
    }
}
    
int getMessageForMap(Plane* plane, int* airlineID){
    return 1;
}
int endSimulation(Map* map){
    
    City* city;
    unsigned int i;
    unsigned int cities = getVectorSize(map->cities);
    for (i = 0; i < cities; i++) {
        if ( !cityIsSatisfied(getFromVector(map->cities, i))) {
            return CONTINUE_SIM;
        }
    }
    return END_SIM;
}

int cityIsSatisfied(City* city) {
   
    Stock* stock;
    unsigned int i;
    unsigned int stock_size = getVectorSize(city->stock);
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
    unsigned int i;
    unsigned int plane_stock_size = getVectorSize(plane->stocks);

    for (i = 0; i < plane_stock_size; i++) {

        Stock* plane_stock = getFromVector(plane->stocks,i);
        Stock* city_stock;
        unsigned int j;
        unsigned int city_stock_size = getVectorSize(city->stock);
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


void giveDirections(Map* map, Plane plane, ipc_t conn){
    return;
}

void startPhaseTwo(Vector* conns){
    return;
}

