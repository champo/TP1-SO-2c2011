#include <stdlib.h>
#include "models/airline.h"



void freeAirline(Airline* airline) {
    unsigned int i;
    if (airline != NULL) {
        free(airline->planes);
        free(airline);
    }
}
