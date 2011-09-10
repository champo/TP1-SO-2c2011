#include "models/airline.h"

#include <stdlib.h>

#include "models/stock.h"

void freeAirline(Airline* airline) {

    if (airline != NULL) {
        for (size_t i = 0; i < airline->numberOfPlanes; i++) {
            freeStocks(airline->planes[i].stocks);
        }

        free(airline->planes);
        free(airline);
    }
}
