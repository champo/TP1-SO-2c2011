#include <stdlib.h>
#include "models/plane.h"


void freePlane(Plane* plane) {
    if ( plane != NULL) {
        freeStocks(plane->stocks);
        free(plane);
    }
}
