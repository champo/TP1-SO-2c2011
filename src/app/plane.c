#include "app/plane.h"
#include "app/airline.h"
#include "communication/plane.h"

#include <stdlib.h>

#define CHECK_EXIT(r) {if((r) == -1) {pthread_exit(0);}}

static int unload_stock(struct PlaneThread* self);

void run_plane(struct PlaneThread* self) {
    int destinations[5], distances[5];
    size_t len;

    mprintf("Hi :d %d\n", self->plane->id);
    while (self->done == 0 && comm_step(self) != -1) {

        mprintf("Waking up for phase 1 (id %d)\n", self->plane->id);
        if (self->plane->distance == 0) {
            CHECK_EXIT(unload_stock(self));
        }

        app_airline_plane_ready();
        CHECK_EXIT(comm_continue(self));

        if (self->plane->distance == 0) {
            len = 5;
            CHECK_EXIT(comm_check_destinations(self, destinations, distances, &len));
            self->plane->cityId = *destinations;
            self->plane->distance = *distances;
        } else {
            self->plane->distance--;
        }
        app_airline_plane_ready();
    }

    self->done = 1;
}

int unload_stock(struct PlaneThread* self) {

    int* stockDelta = NULL;
    Vector* stocks = self->plane->stocks;
    size_t stockLen = getVectorSize(stocks);

    stockDelta = malloc(sizeof(int) * stockLen);
    if (comm_unload_stock(self, stockDelta) == -1) {
        free(stockDelta);
        return -1;
    }

    int done = 1;
    for (size_t i = 0; i < stockLen; i++) {
        Stock* stock = (Stock*) getFromVector(stocks, i);
        stock->amount = stockDelta[i];
        if (stock->amount > 0) {
            done = 0;
        }
    }
    free(stockDelta);

    if (done) {
        self->done = 1;
    }

    return 0;
}

