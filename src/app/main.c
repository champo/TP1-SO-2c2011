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
#include "app/map.h"

#define PARENT_NAME "map_ipc"

void run_airlines(Map* map, Vector* airlines);

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
            Airline* line;
            FILE* file;
            sprintf(path, "%s/%s", argv[1], entry->d_name);
            file = fopen(path, "r");
            line = parseAirlines(file, map);
            line->id = addToVector(airlines, line);
            fclose(file);
        }
    }
    closedir(config);

    run_airlines(map, airlines);
    ipc_listen(PARENT_NAME);

    Vector* conns = createVector();
    size_t numAirlines = getVectorSize(airlines);
    for (size_t i = 0; i < numAirlines; i++) {
        sprintf(path, "%d_air", (int) i);
        addToVector(conns, ipc_establish(path));
    }

    runMap(map, airlines, conns);

    for (size_t i = 0; i < numAirlines; i++) {
        wait(0);
        freeAirline(getFromVector(airlines, i));
        ipc_close(getFromVector(conns, i));
    }

    destroyVector(conns);
    destroyVector(airlines);

    freeMap(map);

    cleanup();
}

void cleanup(void) {
    ipc_end();
    mprintf_end();

    exit(0);
}

void run_airlines(Map* map, Vector* airlines) {

    size_t count = getVectorSize(airlines);
    char name[512];
    ipc_t conn;

    for (size_t i = 0; i < count; i++) {

        Airline* self = getFromVector(airlines, i);
        sprintf(name, "%d_air", self->id);
        if (fork() == 0) {

            freeMap(map);

            for (size_t j = 0; j < count; j++) {
                if (j != i) {
                    freeAirline(getFromVector(airlines, j));
                }
            }
            destroyVector(airlines);

            ipc_listen(name);
            conn = ipc_establish(PARENT_NAME);
            run_airline(self, conn);
            ipc_close(conn);

            mprintf("Bye folks! %d\n", getpid());

            cleanup();
        }
    }
}

