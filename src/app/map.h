#ifndef __APP__MAP__
#define __APP__MAP__



int getMessageForMap(Plane* plane, int* airlineID);
int needDrugs(Map* map);
void updateMap(Map* map, Plane* plane);
void sendPlaneInfo(Plane* plane, ipc_t* conn);
void giveDirections(Map* map, Plane* plane, ipc_t* conn);
void startPhaseTwo(Vector* conns);
void runMap(Map* map, Vector* airlines, Vector* conns);


#endif
