#include "ipc/ipc.h"
#include "models/plane.h"
#include "models/city.h"
#include "models/map.h"
#include "communication/map.h"
#include "communication/plane.h"
#include "marshall/plane.h"
#include "marshall/map.h"
#include "communication/msgqueue.h"


int comm_unloaded_stock(Plane* plane, ipc_t conn){
    
    struct StockStateMessage msg;
    msg = marshall_change_stock(plane->id, plane->stocks);
    transmit(conn, msg);

    return 0;
}

int comm_start_phase_two(Vector* conns){
    
    enum MapMessageType msg = StartPhaseTwo;
    unsigned i;

    for ( i=0; i<getVectorSize(conns); i++) {
        transmit(getFromVector(conns,i),msg);
    }    
    
    return 0;
}
