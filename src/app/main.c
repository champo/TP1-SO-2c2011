#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "util.h"
#include "ipc/ipc.h"
#include "models/airline.h"
#include "app/signal.h"
#include "app/airline.h"
#include "utils/vector.h"
#include "parser.h"

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

    register_exit_function(cleanup);
    register_signal_handlers();

    Map* map;
    Vector* airlines = createVector();
    DIR* config = opendir(argv[1]);
    struct dirent* entry;
    char path[512];

    if (NULL == config) {
        perror("Failed opening the config folder.");
        mprintf_end();
        ipc_end();
        abort();
    }

    sprintf(path, "%s/map", argv[1]);
    map = parseMap(path);
    while ((entry = readdir(config)) != NULL) {
        if (strncmp(entry->d_name, "airline_", 8) == 0) {
            FILE* file;
            sprintf(path, "%s/%s", argv[1], entry->d_name);
            file = fopen(path, "r");
            addToVector(airlines, parseAirlines(file, map));
            fclose(file);
        }
    }
    closedir(config);

    run_airlines(airlines);
    ipc_listen(PARENT_NAME);

    cleanup();
}

void cleanup(void) {
    ipc_end();
    mprintf_end();

    exit(0);
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
            run_airline(self, conn);
            ipc_close(conn);

            cleanup();
        }
    }
}

