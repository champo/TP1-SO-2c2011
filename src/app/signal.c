#include "app/signal.h"

#include <signal.h>
#include <stddef.h>
#include <pthread.h>
#include <stdlib.h>

#include "util.h"

#define HANDLE(sig) signal(sig, signal_handler)

static void signal_handler(int sig);

static void (*exitFunction)(void) = NULL;

void register_exit_function(void (*onExit)(void)) {
    exitFunction = onExit;
}

void register_signal_handlers(void) {
    HANDLE(SIGHUP);
    HANDLE(SIGINT);
    HANDLE(SIGQUIT);
    HANDLE(SIGTERM);
    HANDLE(SIGCHLD);
    HANDLE(SIGILL);
    HANDLE(SIGFPE);
    HANDLE(SIGSEGV);
    HANDLE(SIGPIPE);
}

#define CASE_SIG(sig) case sig: \
    mprintf("Caugh signal "#sig"\n"); \
    abort(); \
    break;

void signal_handler(int sig) {
    // Things just go boom
#ifdef DEBUG
    switch (sig) {
        CASE_SIG(SIGILL)
        CASE_SIG(SIGFPE)
        CASE_SIG(SIGSEGV)
        CASE_SIG(SIGPIPE)
        case SIGHUP:
            mprintf("Caught signal SIGHUP\n");
            break;
        case SIGINT:
            mprintf("Caught signal SIGINT\n");
            break;
        case SIGQUIT:
            mprintf("Caught signal SIGQUIT\n");
            break;
        case SIGTERM:
            mprintf("Caught signal SIGTERM\n");
            break;
        case SIGCHLD:
            mprintf("Caught signal SIGCHLD\n");
            break;
        default:
            mprintf("Caught signal %d\n", sig);
            break;
    }
#ifdef VERBOSE
    print_trace();
#endif
#endif
    if (exitFunction) {
        exitFunction();
    }
}

