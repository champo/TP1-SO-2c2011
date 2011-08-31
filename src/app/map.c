#include <stdlib.h>
#include "models/map.h"
#include "models/airline.h"
#include "ipc/ipc.h"
#include "models/plane.h"
#include "models/city.h"

int getMessageForMap(Plane* plane, int* airlineID);
int needDrugs(Map* map);
void updateMap(Map* map, Plane* plane);
void sendPlaneFromMap(Plane* plane, ipc_t* conn);
void giveDirections(Map* map, Plane* plane, ipc_t* conn);
void startPhaseTwo(Vector* conns);

void runMap(Map* map, Vector* airlines, Vector* conns){
    
    int i,temp,airlinesize,airlineID;
    Plane* curplane;

    if ( (curplane = malloc(sizeof(Plane))) == NULL) {
        return; //avisamo de error che?
    }

    airlinesize = getVectorSize(airlines);
    i = 0;
    
    while (needDrugs(map)) {
            while (i != airlinesize) {
                temp = getMessageForMap(curplane, &airlineID); 
                                                   //returns 0 if it has read a plane which wants to discharge,
                                                   // 1 if it has read an end airline message
                                                   // -1 any other choice
                
                if (temp == 1) {
                    i++;
                }
                if (temp == 0) {
                    updateMap(map, curplane);
                    sendPlaneFromMap(curplane, (ipc_t*)getFromVector(conns,airlineID));
                }
            }

            i = 0;
            
            startPhaseTwo(conns);

            while (i != airlinesize) {
                temp = getMessageForMap(curplane, &airlineID); 
                                                  //returns 0 if it has read a plane which wants instructions,
                                                   // 1 if it has read an end airline message
                                                   // -1 any other choice
                
                if (temp == 1) {
                    i++;
                }
                if (temp == 0) {
                    giveDirections(map, curplane, (ipc_t*)getFromVector(conns,airlineID));
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
    return;
}
void sendPlaneFromMap(Plane* plane, ipc_t* conn){
    return;
}
void giveDirections(Map* map, Plane* plane, ipc_t* conn){
    return;
}
void startPhaseTwo(Vector* conns){
    return;
}

