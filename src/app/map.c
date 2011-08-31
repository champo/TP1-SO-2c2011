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
                    updateMap(map, curplane);
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
void updateMap(Map* map, Plane plane){
    return;
}
<<<<<<< HEAD
void sendPlaneInfo(Plane* plane, ipc_t conn){
=======
void sendPlaneInfo(Plane* plane, ipc_t* conn){
>>>>>>> 234601aa268aad678a21b7b51ccd733ecefc8b92
    return;
}
void giveDirections(Map* map, Plane plane, ipc_t conn){
    return;
}
void startPhaseTwo(Vector* conns){
    return;
}

