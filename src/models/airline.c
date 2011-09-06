#include <stdlib.h>
#include "models/airline.h"



void freeAirline(Airline* airline) {
    if (airline != NULL) {
        free(airline->planes);
        free(airline);
    }
}
