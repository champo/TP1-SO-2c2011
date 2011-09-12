#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "util.h"
#include "ipc/ipc.h"
#include "models/airline.h"
#include "app/signal.h"
#include "app/airline.h"
#include "app/map.h"
#include "app/output.h"
#include "utils/vector.h"
#include "parser.h"
#include "communication/map.h"
#include "communication/msgqueue.h"
#include <pthread.h>

#define PARENT_NAME "map_ipc"

struct MapData {
    Vector* conns;
    Vector* airlines;
    Map* map;
    int* exitState;
    struct MessageQueue* outputMsgQueue;
};

struct OutputData {
    struct MessageQueue* outputMsgQueue;
};

static void run_airlines(Map* map, Vector* airlines);

static void do_map(Map* map, Vector* conns, Vector* airlines, struct MessageQueue* outputMSgQueue);

static void handle_signal(void);

static void start_map(struct MapData* data);

static void start_output(struct OutputData* data); 

static int start_simulation(Map* map, Vector* conns, Vector* airlines, struct MessageQueue* outputMsgQueue);

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

    register_exit_function(handle_signal);
    register_signal_handlers();

    run_airlines(map, airlines);
    ipc_listen(PARENT_NAME);

    Vector* conns = createVector();
    size_t numAirlines = getVectorSize(airlines);
    for (size_t i = 0; i < numAirlines; i++) {
        sprintf(path, "%d_air", (int) i);
        addToVector(conns, ipc_establish(path));
    }

    if ( start_simulation(map, conns, airlines, outputMsgQueue) == -1) {
        
    }

    mprintf("Broadcasting exit to children\n");
    comm_end(conns);

    for (size_t i = 0; i < numAirlines; i++) {
        int status;
        while (wait(&status) == -1 && errno == EINTR);

        if (WIFSIGNALED(status)) {
            mprintf("Child died cause of %d\n", WTERMSIG(status));
        }
    }
    mprintf("All your base belong to us\n");

    for (size_t i = 0; i < numAirlines; i++) {
        freeAirline(getFromVector(airlines, i));
        ipc_close(getFromVector(conns, i));
    }
    
    destroyVector(conns);
    destroyVector(airlines);

    freeMap(map);

    ipc_end();
    mprintf_end();
}

void do_map(Map* map, Vector* conns, Vector* airlines, struct MessageQueue* outputMsgQueue) {

    pthread_t mapThread;
    pthread_attr_t attr;
    struct MapData data = {
        .map = map,
        .airlines = airlines,
        .conns = conns,
        .exitState = &doExit,
        .outputMsgQueue = outputMsgQueue
    };

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&mapThread, &attr, start_map, &data);
    pthread_join(mapThread, NULL);

    pthread_attr_destroy(&attr);
}

void start_map(struct MapData* data) {
    runMap(data->map, data->airlines, data->conns, data->exitState, data->outputMsgQueue);
}

void handle_signal(void) {
    doExit = 1;
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


static int start_simulation(Map* map, Vector* conns, Vector* airlines, struct MessageQueue* outputMsgQueue) {
    
    pthread_t outputThread;    
    pthread_attr_t attr;
    struct MessageQueue* outputMsgQueue;
    
    if ((outputMsgQueue = message_queue_create()) == NULL) {
        mprintf("Output msg queue creation failed... aborting...\n");
        return -1;
    }
    
    struct OutputData data = {
        .outputMsgQueue = outputMsgQueue
    };

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&outputThread, &attr, start_output, &data);
    
    do_map(map, conns, airlines, outputMsgQueue);

    comm_end_output(outputMsgQueue);
    message_queue_destroy(outputMsgQueue);

    pthread_join(outputThread, NULL);
    pthread_attr_destroy(&attr);
}

void start_output(struct OutputData* data) {
    run_output(data->outputMsgQueue);
}
