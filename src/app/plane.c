#include "app/plane.h"

#define CHECK_EXIT(r) {if((r) == -1) {pthread_exit(0);}}

static int step(struct PlaneThread* self);

void run_plane(struct PlaneThread* self) {
    int finished = 0;
    int destinations[5];
    int target;
    size_t len;
    while (step(self)) {
        len = 5;
        CHECK_EXIT(comm_continue(self));
        CHECK_EXIT(comm_check_destinations(self, destinations, &len));
        target = *destinations;
        CHECK_EXIT(comm_set_destination(self, target));
    }
    self->done = 1;
}

int step(struct PlaneThread* self) {
    int* stockDelta = NULL;
    Vector* stocks = self->plane->stocks;
    size_t len = getVectorSize(stocks);
    int done = 1;

    if (comm_step(self, stockDelta) == -1) {
        return 0;
    }

    for (int i = 0; i < len; i++) {
        Stock* stock = (Stock*) getFromVector(stocks, i);
        stock->amount -= stockDelta[i];
        if (stock->amount > 0) {
            done = 0;
        }
    }

    return !done;
}

