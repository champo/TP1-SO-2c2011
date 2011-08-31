#include <stdlib.h>
#include "ipc/ipc.h"
#include "models/plane.h"
#include "models/city.h"
#include "models/map.h"
#include "models/airline.h"
#include "app/map.h"

void runMap(Map* map, Vector* airlines, Vector* conns){
    
    int i,temp,airlinesize,airlineID;
    Plane curplane;

    airlinesize = getVectorSize(airlines);
    i = 0;
    
    while (needDrugs(map)) {
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
                    sendPlaneInfo(curplane, (ipc_t)getFromVector(conns,airlineID));
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
int needDrugs(Map* map){
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

void sendPlaneInfo(Plane plane, ipc_t conn){
    return;
}
void giveDirections(Map* map, Plane plane, ipc_t conn){
    return;
}
void startPhaseTwo(Vector* conns){
    return;
}

