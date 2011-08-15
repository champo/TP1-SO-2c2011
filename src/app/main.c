#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "util.h"
#include "ipc/ipc.h"
#include "models/airline.h"

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
    //TODO: Fork all the airline processes and setup the map thread

    ipc_end();
    mprintf_end();
}

