#include <sys/types.h>
#include <unistd.h>

#include "util.h"
#include "ipc/ipc.h"
#include "models/airline.h"

int main(int argc, char *argv[]) {
    mprintf_init();
    ipc_init();

    //TODO: Parse everything, and get the proper data structures
    //TODO: Position the planes on their initial places on the map
    //TODO: Fork all the airline processes and setup the map thread

    ipc_end();
    mprintf_end();
}

