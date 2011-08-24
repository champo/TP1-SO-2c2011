#include <stdlib.h>
#include "models/airline.h"



void freeAirline(Airline* airline) {
    unsigned int i;
    if (airline != NULL) {
        for (i = 0; i < airline->numberOfPlanes; i++) {
            freePlane(&(airline->planes[i]));
        }
        free(airline);
    }
}
