#include "app/plane.h"
#include "app/airline.h"
#include "communication/plane.h"

#include <stdlib.h>

#define CHECK_EXIT(r) {if((r) == -1) {print_error("Read invalid message in plane\n");print_trace();pthread_exit(0);}}
#define echo(msg) mprintf("[%d] "msg"\n", self->plane->id)

static int unload_stock(struct PlaneThread* self);

void run_plane(struct PlaneThread* self) {
    int destinations[5], distances[5];
    size_t len;

    echo("Preparing to start");
    while (self->done == 0 && comm_step(self) != -1) {

        echo("Waking for phase 1");
        if (self->plane->distance == 0) {
            int i = unload_stock(self);
            CHECK_EXIT(i);
        }

        app_airline_plane_ready();
        if (self->done) {
            echo("I unloaded everything, so I'm bialing");
            break;
        }
        echo("Waiting for phase2...");
        CHECK_EXIT(comm_continue(self));

        if (self->plane->distance == 0) {
            len = 5;
            CHECK_EXIT(comm_check_destinations(self, destinations, distances, &len));
            if (len == 0) {
                echo("Done early dude!");
                self->done = 1;
            } else {
                mprintf("[%d] Goint from %d to %d with distance %d\n",
                        self->plane->id, self->plane->cityId, *destinations, *distances);
                self->plane->cityId = *destinations;
                self->plane->distance = *distances;
            }
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
    if (stockDelta == NULL || comm_unload_stock(self, stockDelta) == -1) {
        free(stockDelta);
        return -1;
    }

    int done = 1;
    for (size_t i = 0; i < stockLen; i++) {
        Stock* stock = (Stock*) getFromVector(stocks, i);
        stock->amount = stockDelta[i];
        if (stock->amount > 0) {
            done = 0;
        } else if (stock->amount < 0) {
            print_error("Got negative stock when unloading\n");
            return -1;
        }
    }
    free(stockDelta);

    if (done) {
        self->done = 1;
    }

    return 0;
}

