#include "app/signal.h"

#define _POSIX_SOURCE

#include <signal.h>
#include <stddef.h>
#include <pthread.h>
#include <stdlib.h>

#include "util.h"

#define HANDLE(sig) signal(sig, signal_handler)

static void signal_handler(int sig);

static void redirect_handler(int sig);

static void (*exitFunction)(void) = NULL;

void register_exit_function(void (*onExit)(void)) {
    exitFunction = onExit;
}

void redirect_signals(void) {
    signal(SIGHUP, redirect_handler);
    signal(SIGINT, redirect_handler);
    signal(SIGQUIT, redirect_handler);
    signal(SIGTERM, redirect_handler);
}

void redirect_handler(int sig) {
    kill(getppid(), sig);
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
    printf("Caugh signal "#sig"\n"); \
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
            printf("Caught signal SIGHUP\n");
            break;
        case SIGINT:
            printf("Caught signal SIGINT\n");
            break;
        case SIGQUIT:
            printf("Caught signal SIGQUIT\n");
            break;
        case SIGTERM:
            printf("Caught signal SIGTERM\n");
            break;
        case SIGCHLD:
            printf("Caught signal SIGCHLD\n");
            break;
        default:
            printf("Caught signal %d\n", sig);
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

