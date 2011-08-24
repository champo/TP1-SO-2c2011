#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "util.h"
#include "ipc/ipc.h"
#include "models/airline.h"
#include "app/signal.h"
#include "utils/vector.h"
#include "parser.h"
#include "models/map.h"
#include "models/city.h"

#define PARENT_NAME "map_ipc"

void run_airlines(Vector* airlines);

void cleanup(void);

int main(int argc, char *argv[]) {
   
    if (argc != 2) {
        printf("Usage: %s configFolder\n", argv[0]);
        exit(0);
    }

    if (!mprintf_init()) {
        printf("Init failed... aborting...\n");
        abort();
    }

    if (ipc_init() == -1) {
        printf("IPC init failed... aborting...\n");
        mprintf_end();
        abort();
    }

    //TODO: Parse everything, and get the proper data structures
    //TODO: Position the planes on their initial places on the map
    register_exit_function(cleanup);
    register_signal_handlers();
    //TODO: Register an exit handler
    Vector* airlines = NULL;
    //run_airlines(airlines);
    ipc_listen(PARENT_NAME);

    cleanup();

}

void cleanup(void) {
    ipc_end();
    mprintf_end();
}

void run_airlines(Vector* airlines) {

    size_t count = getVectorSize(airlines);
    char name[512];
    ipc_t conn;

    for (size_t i = 0; i < count; i++) {

        Airline* self = getFromVector(airlines, i);
        sprintf(name, "airline_%d", self->id);
        if (fork()) {

            // This is the creator process
            conn = ipc_establish(name);
            // TODO: Store this somewhere
        } else {

            //FIXME: I'm gonna leak the map! Yay! Good for me! :D

            for (size_t j = 0; j < count; j++) {
                if (j != i) {
                    //TODO: Free the airline
                }
            }
            destroyVector(airlines);

            ipc_listen(name);
            conn = ipc_establish(PARENT_NAME);
            run_airline(self, conn, cleanup);
            ipc_close(conn);

            cleanup();
            exit(0);
        }
    }
}

