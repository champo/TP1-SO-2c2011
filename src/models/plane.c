#include <stdlib.h>
#include "models/plane.h"


void freePlane(Plane* plane) {
    if ( plane != NULL) {
        freeStock(plane->stocks);
        free(plane);
    }
}