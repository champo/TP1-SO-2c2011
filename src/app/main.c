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

Vector* run_airlines(Vector* airlines);

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
    register_signal_handlers();
    //TODO: Register an exit handler
    Vector* airlines = NULL;
    Vector* connections = run_airlines(airlines);

    cleanup();

}

void cleanup(void) {
    ipc_end();
    mprintf_end();
}

Vector* run_airlines(Vector* airlines) {

    size_t count = getVectorSize(airlines);
    Vector* conns = createVector();

    pid_t pid;
    ipc_t conn;

    for (size_t i = 0; i < count; i++) {

        conn = ipc_create();
        if ((pid = fork())) {
            // This is the creator process
            ipc_establish(conn, pid);
            addToVector(conns, conn);
        } else {
            // We don't need the vector, so we scrap it
            Airline* self = getFromVector(airlines, i);
            size_t connCount = getVectorSize(conns);
            for (size_t j = 0; j < connCount; j++) {
                ipc_discard((ipc_t) getFromVector(conns, j));
            }
            destroyVector(conns);
            //FIXME: I'm gonna leak the map! Yay! Good for me! :D

            for (size_t j = 0; j < count; j++) {
                if (j != i) {
                    //TODO: Free the airline
                }
            }
            destroyVector(airlines);

            ipc_establish(conn, pid);
            run_airline(self, conn, cleanup);
            ipc_close(conn);

            cleanup();
            exit(0);
        }
    }
}

