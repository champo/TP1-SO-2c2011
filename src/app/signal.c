#include "app/signal.h"

static void (*exitFunction)(void);

void register_exit_function(void (*onExit)(void)) {
    exitFunction = onExit;
}

void register_signal_handlers(void) {
    //TODO: I'm a stub :D
}

