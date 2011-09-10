#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "ipc/ipc.h"
#include "models/airline.h"
#include "app/signal.h"
#include "app/airline.h"
#include "utils/vector.h"
#include "parser.h"
#include "app/map.h"
#include "communication/map.h"

#include <pthread.h>

#define PARENT_NAME "map_ipc"

struct MapData {
    Vector* conns;
    Vector* airlines;
    Map* map;
    int* exitState;
    pthread_mutex_t* exitLock;
};

static void run_airlines(Map* map, Vector* airlines);

static void do_map(Map* map, Vector* conns, Vector* airlines);

static void do_exit(void);

static void handle_signal(int sig);

static void start_map(struct MapData* data);

static pthread_cond_t exitWait = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t exitLock = PTHREAD_MUTEX_INITIALIZER;

static int doExit = 0;

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


    pthread_mutex_lock(&exitLock);
    register_exit_function(handle_signal);
    register_signal_handlers();

    Map* map;
    Vector* airlines = createVector();
    DIR* config = opendir(argv[1]);
    struct dirent* entry;
    char path[512];

    if (NULL == config) {
        print_errno("Failed opening the config folder.");
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

    do_map(map, conns, airlines);
    mprintf("Broadcasting exit to children\n");
    comm_end(conns);

    for (size_t i = 0; i < numAirlines; i++) {
        while (wait(0) == -1 && errno == EINTR);
    }

    for (size_t i = 0; i < numAirlines; i++) {
        freeAirline(getFromVector(airlines, i));
        ipc_close(getFromVector(conns, i));
    }

    destroyVector(conns);
    destroyVector(airlines);

    freeMap(map);

    pthread_mutex_unlock(&exitLock);
    ipc_end();
    mprintf_end();
}

void do_map(Map* map, Vector* conns, Vector* airlines) {

    pthread_t mapThread;
    struct MapData data = {
        .map = map,
        .airlines = airlines,
        .conns = conns,
        .exitState = &doExit,
        .exitLock = &exitLock
    };

    pthread_create(&mapThread, NULL, start_map, &data);

    while (doExit == 0) {
        pthread_cond_wait(&exitWait, &exitLock);
    }

    pthread_cancel(mapThread);
}

void start_map(struct MapData* data) {

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    runMap(data->map, data->airlines, data->conns, data->exitState, data->exitLock);

    do_exit();
}

void handle_signal(int sig) {
    do_exit();
}

void do_exit(void) {
    doExit = 1;
    pthread_cond_signal(&exitWait);
}

void run_airlines(Map* map, Vector* airlines) {

    size_t count = getVectorSize(airlines);
    char name[512];
    ipc_t conn;

    for (size_t i = 0; i < count; i++) {

        Airline* self = getFromVector(airlines, i);
        sprintf(name, "%d_air", self->id);
        if (fork() == 0) {

            ipc_listen(name);
            conn = ipc_establish(PARENT_NAME);
            run_airline(self, conn);
            ipc_close(conn);

            freeMap(map);
            for (size_t j = 0; j < count; j++) {
                freeAirline(getFromVector(airlines, j));
            }
            destroyVector(airlines);

            mprintf("Bye folks! %d\n", getpid());

            ipc_end();
            exit(0);
        }
    }
}

