#include "app/plane.h"
#include "communication/plane.h"

#include <stdlib.h>

#define CHECK_EXIT(r) {if((r) == -1) {pthread_exit(0);}}

static int unload_stock(struct PlaneThread* self);

void run_plane(struct PlaneThread* self) {
    int finished = 0;
    int destinations[5];
    int target;
    size_t len;

    while (self->done == 0 && comm_step(self) != -1) {

        if (self->plane->distance == 0) {
            CHECK_EXIT(unload_stock(self));
        } else {
            CHECK_EXIT(comm_intransit(self));
        }

        CHECK_EXIT(comm_continue(self));

        if (self->plane->distance == 0) {
            len = 5;
            CHECK_EXIT(comm_check_destinations(self, destinations, &len));
            target = *destinations;
            CHECK_EXIT(comm_set_destination(self, target));
        } else {
            self->plane->distance--;
        }
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
    for (int i = 0; i < stockLen; i++) {
        Stock* stock = (Stock*) getFromVector(stocks, i);
        stock->amount -= stockDelta[i];
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

